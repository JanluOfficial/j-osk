#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace j_osk {

enum class KeyType { Text, Modifier };

struct LayoutKey {
  std::string label;
  std::string action;
  KeyType type;
  std::optional<float> width;
};

struct LayoutRow {
  std::vector<LayoutKey> keys;
};

struct LayoutMetadata {
  std::string name;
  std::string author;
  int cformat = 0;
  bool split = false;
};

struct Layout {
  LayoutMetadata metadata;
  std::vector<LayoutRow> rows;
};

class LayoutParser {
public:
  static Layout parseFile(const std::filesystem::path &path);
  static Layout parseJson(const nlohmann::json &json);

private:
  static LayoutMetadata parseMetadata(const nlohmann::json &metadataJson);
  static LayoutRow parseRow(const nlohmann::json &rowJson);
  static LayoutKey parseKey(const nlohmann::json &keyJson);
};

} // namespace j_osk