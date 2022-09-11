// Черепухин Евгений Сергеевич. Спринт 14 Версия 1. 
#pragma once

#include "json.h"

#include <memory>
#include <string>
#include <exception>
#include <vector>
#include <optional>
#include <stdexcept>

namespace json {

	class DictItemContext;
	class ArrayItemContext;
	class KeyValueContext;

	using namespace std::string_literals;
	class Builder {
	public:
		Builder();
		~Builder();

		KeyValueContext Key(const std::string& key);

		Builder& Value(Node::Value value);

		DictItemContext StartDict();

		ArrayItemContext StartArray();


		Builder& EndDict();

		Builder& EndArray();

		Node Build();


	private:
		Node root_;
		std::vector<Node*> nodes_stack_;
		std::optional<std::string> key_;

		struct NodeConstructor {
			Node operator() (std::nullptr_t) const {
				return Node();
			}
			Node operator() (std::string value) const {
				return Node(std::move(value));
			}
			Node operator() (bool value) const {
				return Node(value);
			}
			Node operator() (int value) const {
				return Node(value);
			}
			Node operator() (double value) const {
				return Node(value);
			}
			Node operator() (Array value) const {
				return Node(std::move(value));
			}
			Node operator() (Dict value) const {
				return Node(std::move(value));
			}
		};
	};

	class ArrayItemContext {
	public:
		ArrayItemContext(Builder& builder);
		ArrayItemContext Value(Node::Value value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndArray();
	private:
		Builder& builder_;
	};

	class KeyValueContext {
	public:
		KeyValueContext(Builder& builder);
		DictItemContext Value(Node::Value value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
	private:
		Builder& builder_;
	};

	class DictItemContext {
	public:
		DictItemContext(Builder& builder);
		KeyValueContext Key(std::string key);
		Builder& EndDict();
	private:
		Builder& builder_;
	};

}