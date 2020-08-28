#include <algine/core/shader/ShaderManager.h>

#include <algine/core/JsonHelper.h>

#include <tulz/Path.h>
#include <tulz/File.h>
#include <tulz/StringUtils.h>

#include <stdexcept>
#include <iostream>

using namespace nlohmann;
using namespace tulz;
using namespace tulz::StringUtils;
using namespace std;

#define constant(name, val) constexpr char name[] = val

// ALP means Algine Preprocessor
namespace ALPKeywords {
constant(Include, "include");
constant(Link, "link");
}

namespace Config {
constant(Type, "type");

constant(Vertex, "vertex");
constant(Fragment, "fragment");
constant(Geometry, "geometry");

constant(Source, "source");
constant(Path, "path");

constant(IncludePaths, "includePaths");
}

namespace algine {
template<typename T>
inline bool isElementExist(const vector<T> &v, const T &e) {
    return find(v.begin(), v.end(), e) != v.end();
}

template<typename T>
inline void removeElement(vector<T> &v, const T &e) {
    auto it = find(v.begin(), v.end(), e);

    if (it != v.end()) {
        v.erase(it);
    }
}

vector<string> ShaderManager::m_globalIncludePaths;

ShaderManager::ShaderManager()
    : m_type(),
      m_dumperUseSources(false)
{
    // see initializer list above
}

ShaderManager::ShaderManager(uint type)
    : m_type(type),
      m_dumperUseSources(false)
{
    // see initializer list above
}

void ShaderManager::fromFile(const string &path) {
    m_path = path;

    // base include path
    m_includePaths.insert(m_includePaths.begin(), Path(path).getParentDirectory());

    fromSource(File(path, File::Read).readStr());
}

void ShaderManager::fromSource(const string &source) {
    m_source = source;
    resetGenerated();
}

void ShaderManager::setIncludePaths(const vector<string> &includePaths) {
    m_includePaths = includePaths;
}

void ShaderManager::addIncludePaths(const vector<string> &includePaths) {
    for (const auto & i : includePaths) {
        addIncludePath(i);
    }
}

void ShaderManager::addIncludePath(const string &includePath) {
    if (!isElementExist(m_includePaths, includePath)) {
        m_includePaths.emplace_back(includePath);
    }
}

void ShaderManager::removeIncludePath(const std::string &includePath) {
    removeElement(m_includePaths, includePath);
}

void ShaderManager::resetGenerated() {
    m_gen = "";
}

void ShaderManager::generate() {
    constexpr char versionRegex[] = R"~([ \t]*#[ \t]*version[ \t]+[0-9]+(?:[ \t]+[a-z]+|[ \t]*)(?:\r\n|\n|$))~";

    m_gen = m_source;

    // generate definitions code
    vector<Matches> version = findRegex(m_gen, versionRegex);
    if (version.empty())
        return;

    string definitionsCode = "\n";
    for (auto & j : m_definitions)
        definitionsCode += "#define " + j.first + " " + j.second + "\n";
    m_gen.insert(version[0].pos + version[0].size, definitionsCode);

    // expand includes
    // base include path (path where file is located)
    // it is zero element if shader loaded from file
    if (!m_path.empty() && !m_includePaths.empty()) {
        m_gen = processDirectives(m_gen, m_includePaths[0]);
    } else {
        m_gen = processDirectives(m_gen, "");
    }
}

void ShaderManager::setType(uint type) {
    m_type = type;
}

string ShaderManager::getConfigPath() const {
    return m_confPath;
}

uint ShaderManager::getType() const {
    return m_type;
}

const vector<string>& ShaderManager::getIncludePaths() const {
    return m_includePaths;
}

string ShaderManager::getTemplate() {
    return m_source;
}

string ShaderManager::getGenerated() {
    return m_gen;
}

string ShaderManager::makeGenerated() {
    generate();
    return getGenerated();
}

ShaderPtr ShaderManager::createShader() {
    generate();

    ShaderPtr shader = make_shared<Shader>(m_type);
    shader->fromSource(m_gen);
    shader->setName(m_name);

    if (m_access == Access::Public) {
        if (m_name.empty())
            throw runtime_error("Shader without name can't be public");

        if (Shader::byName(m_name) == nullptr) {
            Shader::publicShaders.emplace_back(shader);
        } else {
            throw runtime_error("Shader with the same name was already loaded");
        }
    }

    return shader;
}

void ShaderManager::dumperUseSources(bool use) {
    m_dumperUseSources = use;
}

void ShaderManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load shader path or source
    if (config.contains(Source)) {
        fromSource(config[Source]);
    } else if (config.contains(Config::Path)) {
        fromFile(config[Config::Path]);
    } else {
        throw runtime_error("ShaderManager: broken file:\n" + jsonHelper.toString());
    }

    // load include paths
    if (config.contains(IncludePaths)) {
        const json &includeArray = config[IncludePaths];

        for (const auto & i : includeArray) {
            addIncludePath(i);
        }
    }

    // load type
    m_type = map<string, uint> {
        {Vertex, Shader::Vertex},
        {Fragment, Shader::Fragment},
        {Geometry, Shader::Geometry}
    } [config[Type]];

    ManagerBase::import(jsonHelper);
    ShaderDefinitionManager::import(jsonHelper);
}

JsonHelper ShaderManager::dump() {
    using namespace Config;

    json config;

    auto setString = [&](const string &key, const string &value)
    {
        if (!value.empty())
            config[key] = value;
    };

    // write source or path
    if (m_dumperUseSources) {
        setString(Source, m_source);
    } else {
        setString(Config::Path, m_path);
    }

    // write include paths
    if (!m_includePaths.empty())
        config[IncludePaths] = m_includePaths;

    // write type
    // note: Shader must keep types order
    config[Type] = vector<string> {Vertex, Fragment, Geometry} [m_type];

    JsonHelper result(config);
    result.append(ManagerBase::dump());
    result.append(ShaderDefinitionManager::dump());

    return result;
}

void ShaderManager::importFromFile(const string &path) {
    m_confPath = path;
    import(File(path, File::ReadText).readStr());
}

void ShaderManager::setGlobalIncludePaths(const vector<string> &includePaths) {
    m_globalIncludePaths = includePaths;
}

void ShaderManager::addGlobalIncludePaths(const vector<string> &includePaths) {
    for (const auto & i : includePaths) {
        addGlobalIncludePath(i);
    }
}

void ShaderManager::addGlobalIncludePath(const string &includePath) {
    if (!isElementExist(m_globalIncludePaths, includePath)) {
        m_globalIncludePaths.emplace_back(includePath);
    }
}

void ShaderManager::removeGlobalIncludePath(const string &includePath) {
    removeElement(m_globalIncludePaths, includePath);
}

vector<string>& ShaderManager::getGlobalIncludePaths() {
    return m_globalIncludePaths;
}

// src: where to insert
// srcPos: position to start erase
// srcSize: count of symbols to erase
// data: what to insert, will be inserted in srcPos position
inline void insert(string &src, uint srcPos, uint srcSize, const string &data) {
    src = src.erase(srcPos, srcSize);
    src.insert(srcPos, data);
}

inline vector<Matches> findPragmas(const string &src, const string &regex, const vector<pair<uint, uint>> &excludes) {
    auto matches = StringUtils::findRegex(src,
            R"([ \t]*#[ \t]*pragma[ \t]+algine[ \t]+)" + regex +
            R"(|[ \t]*#[ \t]*alp[ \t]+)" + regex);

    for (int i = static_cast<int>(matches.size()) - 1; i >= 0; i--) {
        for (auto &exclude : excludes) {
            if (matches[i].pos > exclude.first && matches[i].pos + matches[i].size <= exclude.first + exclude.second) {
                matches.erase(matches.begin() + i);
            }
        }
    }

    return matches;
}

inline vector<pair<uint, uint>> findComments(const string &src) {
    constexpr char regex[] = R"(//.*|/\*(?:.|\n|\r\n)*?\*/)"; // line and block comments
    auto matches = StringUtils::findRegex(src, regex);
    vector<pair<uint, uint>> result;
    result.reserve(matches.size());

    for (const auto &match : matches)
        result.emplace_back(match.pos, match.size);

    return result;
}

string ShaderManager::processDirectives(const string &src, const string &baseIncludePath) {
    string result = src;
    constexpr char regex[] = R"~((\w+)[ \t]+(.+))~";

    using namespace ALPKeywords;

    // We process from the end because if we start from the beginning -
    // Matches::pos will be violated because of new data insertion
    vector<Matches> pragmas = findPragmas(result, regex, findComments(src));

    for (int j = static_cast<int>(pragmas.size()) - 1; j >= 0; j--) { // if pragmas empty, j can be -1
        Matches &matches = pragmas[j];
        string &pragmaName = matches.matches[1];

        auto pragmaIs = [&](const string &name)
        {
            return pragmaName == name;
        };

        if (pragmaIs(Include)) {
            auto fileMatches = StringUtils::findRegex(matches.matches[2], R"~("(.+)")~"); // "file"
            string &filePath = fileMatches[0].matches[1];

            auto fileNotFoundError = [&]()
            {
                cerr << "ShaderManager: Error: file " << filePath << " not found\n" << matches.matches[0] << "\n\n";
            };

            if (!Path(filePath).isAbsolute()) {
                if (Path(Path::join(baseIncludePath, filePath)).exists()) { // try to find included file in base file folder
                    filePath = Path::join(baseIncludePath, filePath);
                } else {
                    bool found = false;

                    auto findIncludePath = [&](const vector<string> &includePaths)
                    {
                        for (const auto &i : includePaths) { // i - include path
                            if (Path(Path::join(i, filePath)).exists()) {
                                filePath = Path::join(i, filePath);
                                found = true;
                                break;
                            }
                        }
                    };

                    // try to find include path in the own array
                    findIncludePath(m_includePaths);

                    // otherwise try to find include path in the global array
                    if (!found)
                        findIncludePath(m_globalIncludePaths);

                    if (!found) {
                        fileNotFoundError();
                        continue;
                    }
                }
            } else if (!Path(filePath).exists()) {
                fileNotFoundError();
                continue;
            }

            insert(result, matches.pos, matches.size,
                   processDirectives(File(filePath, File::Read).readStr(), Path(filePath).getParentDirectory()));
        } else if (pragmaIs(Link)) {
            auto fileMatches = StringUtils::findRegex(matches.matches[2], R"~((.+)[ \t]+(.+))~");

            // #alp link base link
            insert(result, matches.pos, matches.size,
                   "#define " + fileMatches[0].matches[2] + " " + fileMatches[0].matches[1]);
        } else {
            cerr << "Unknown pragma " << pragmaName << "\n" << matches.matches[0] << "\n\n";
        }
    }

    return result;
}
}
