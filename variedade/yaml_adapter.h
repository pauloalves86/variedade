/**
 * @author Robin McCorkell <rmccorkell@karoshi.org.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef YAMLADAPTER_HPP
#define YAMLADAPTER_HPP

#include <algorithm>
#include <set>
#include <string>
#include <stdexcept>

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/type.h>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/options_description.hpp>

class YamlAdapter {
public:
	YamlAdapter(
		const boost::program_options::options_description& desc,
		bool allow_unregistered = false
	) : allow_unregistered(allow_unregistered), desc(desc)
	{
		for (const auto& option_desc : desc.options()) {
			if (!option_desc->long_name().empty()) {
				allowed_options.insert(option_desc->long_name());
			}
		}
	}

	template <typename CharT = char>
	boost::program_options::basic_parsed_options<CharT> parse(
		const YAML::Node& node
	) {
		boost::program_options::basic_parsed_options<CharT> result(&desc);
		parseSubnode(node, "", result);
		return result;
	}

protected:
	template <typename CharT>
	void parseSubnode(
		const YAML::Node& node,
		const std::string& key,
		boost::program_options::basic_parsed_options<CharT>& result
	) {
		switch (node.Type()) {
		case YAML::NodeType::Scalar:
			addOption(key, node.as<std::basic_string<CharT>>(), result);
			break;
		case YAML::NodeType::Sequence:
			parseSubnodeSequence(node, key, result);
			break;
		case YAML::NodeType::Map:
			parseSubnodeMap(node, key, result);
			break;
		default:
			break;
		}
	}

	template <typename CharT>
	void parseSubnodeSequence(
		const YAML::Node& node,
		const std::string& key,
		boost::program_options::basic_parsed_options<CharT>& result
	) {
		for (const auto& subnode : node) {
			parseSubnode(subnode, key, result);
		}
	}

	template <typename CharT>
	void parseSubnodeMap(
		const YAML::Node& node,
		const std::string& key,
		boost::program_options::basic_parsed_options<CharT>& result
	) {
		for (const auto& pair : node) {
			std::string node_key = pair.first.as<std::string>();
			try {
				std::stoul(node_key); // check if we have an integer
				node_key = key; // treat this as if it were a Sequence
			} catch (std::invalid_argument& e) {
				// not an integer
				if (!key.empty()) {
					node_key = key + '.' + node_key;
				}
			}
			parseSubnode(pair.second, node_key, result);
		}
	}

	template <typename CharT>
	void addOption(
		const std::string& key,
		const std::basic_string<CharT>& value,
		boost::program_options::basic_parsed_options<CharT>& result
	) {
		if (key.empty()) {
			throw std::logic_error("Empty key - malformed YAML?");
		}
		auto allowed_iter = allowed_options.find(key);
		if (!allow_unregistered && allowed_iter == allowed_options.end()) {
			throw std::logic_error("Unallowed option in YAML node");
		}

		auto option_iter = std::find_if(
			result.options.begin(), result.options.end(),
			[&key](const boost::program_options::basic_option<CharT>& test) {
				return test.string_key == key;
			}
		);

		if (option_iter == result.options.end()) {
			result.options.emplace_back();
			option_iter = result.options.end() - 1;
			option_iter->string_key = key;
			if (allowed_iter == allowed_options.end()) {
				option_iter->unregistered = true;
			}
		}

		option_iter->value.push_back(value);
	}

private:
	const boost::program_options::options_description& desc;
	std::set<std::string> allowed_options;
	bool allow_unregistered;
};

#endif