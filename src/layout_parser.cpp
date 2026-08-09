#include "layout_parser.h"

#include <fstream>
#include <stdexcept>

namespace j_osk {

Layout LayoutParser::parseFile(const std::filesystem::path &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open layout file: " + path.string());
  }

  nlohmann::json json;
  file >> json;
  return parseJson(json);
}

Layout LayoutParser::parseJson(const nlohmann::json &json) {
  if (!json.contains("metadata") || !json.contains("layout")) {
    throw std::runtime_error("Layout JSON must contain 'metadata' and 'layout'");
  }

  Layout layout;
  layout.metadata = parseMetadata(json.at("metadata"));

  const auto &layoutJson = json.at("layout");
  if (!layoutJson.contains("rows") || !layoutJson.at("rows").is_array()) {
    throw std::runtime_error("Layout JSON 'layout' must contain an array of 'rows'");
  }

  for (const auto &rowJson : layoutJson.at("rows")) {
    layout.rows.push_back(parseRow(rowJson));
  }

  return layout;
}

LayoutMetadata LayoutParser::parseMetadata(const nlohmann::json &metadataJson) {
  if (!metadataJson.contains("name") || !metadataJson.contains("author")) {
    throw std::runtime_error("Layout metadata must contain 'name' and 'author'");
  }

  LayoutMetadata metadata;
  metadata.name = metadataJson.at("name").get<std::string>();
  metadata.author = metadataJson.at("author").get<std::string>();
  metadata.cformat = metadataJson.value("cformat", 0);
  metadata.split = metadataJson.value("split", false);

  return metadata;
}

LayoutRow LayoutParser::parseRow(const nlohmann::json &rowJson) {
  if (!rowJson.contains("keys") || !rowJson.at("keys").is_array()) {
    throw std::runtime_error("Each layout row must contain an array of 'keys'");
  }

  LayoutRow row;
  for (const auto &keyJson : rowJson.at("keys")) {
    row.keys.push_back(parseKey(keyJson));
  }

  return row;
}

LayoutKey LayoutParser::parseKey(const nlohmann::json &keyJson) {
  if (!keyJson.contains("label") || !keyJson.contains("action")) {
    throw std::runtime_error("Each layout key must contain 'label' and 'action'");
  }

  LayoutKey key;
  key.label = keyJson.at("label").get<std::string>();
  key.action = keyJson.at("action").get<std::string>();

  if (key.action.rfind("mod:", 0) == 0) // Modifier
    key.type = KeyType::Modifier;
  else if (key.action.rfind("text:", 0) == 0) // Text
    key.type = KeyType::Text;

  if (keyJson.contains("width")) {
    key.width = keyJson.at("width").get<float>();
  }

  return key;
}

} // namespace j_osk
