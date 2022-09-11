// Черепухин Евгений Сергеевич. Спринт 14 Версия 1. 
#include "json_builder.h"

namespace json {

	Builder::Builder()
		:nodes_stack_(1, &root_) {}

	Builder::~Builder() {
		if (!nodes_stack_.empty()) {
			nodes_stack_.clear();
		}
	}

	KeyValueContext Builder::Key(const std::string& key) {
		if (root_.IsNull() || nodes_stack_.empty()) {
			throw std::logic_error("Wrong method. Method required Build."s);
		}
		if (key_ || !nodes_stack_.back()->IsDict()) {
			throw std::logic_error("Wrong method. Method required Build."s);
		}
		key_ = std::move(key);
		return *this;
	}

	Builder& Builder::Value(Node::Value value) {
		const auto back_item = nodes_stack_.back();
		if (back_item->IsNull()) {
			back_item->GetValue() = std::move(value);
		}
		else if (back_item->IsArray()) {
			Array& arr = std::get<Array>(back_item->GetValue());
			arr.emplace_back(std::visit(NodeConstructor{}, std::move(value)));
		}
		else if (back_item->IsDict()) {
			if (!key_) {
				throw std::logic_error("Attempt to add a value to a map without a key."s);
			}
			Dict& dict = std::get<Dict>(back_item->GetValue());
			dict.emplace(*key_, std::visit(NodeConstructor{}, std::move(value)));
			key_ = std::nullopt;
		}
		else {
			throw std::logic_error("Wrong method. Method required Build."s);
		}
		return *this;
	}

	DictItemContext Builder::StartDict() {
		if (nodes_stack_.empty()) {
			throw std::logic_error("Wrong method. Method required Build."s);
		}
		const auto back_element = nodes_stack_.back();
		if (back_element->IsNull()) {
			*back_element = Dict();
			nodes_stack_.emplace_back(nodes_stack_.back());
		}
		else if (back_element->IsArray()) {
			Array& arr = std::get<Array>(back_element->GetValue());
			arr.emplace_back(Dict());
			nodes_stack_.emplace_back(&arr.back());
		}
		else if (back_element->IsDict()) {
			if (!key_) {
				throw std::logic_error("Attempt to add a value to a map without a key."s);
			}
			Dict& dict = std::get<Dict>(back_element->GetValue());

			const auto dictionary = dict.emplace(*key_, Dict());
			nodes_stack_.push_back(&dictionary.first->second);
			key_ = std::nullopt;
		}
		else {
			throw std::logic_error("Wrong method. Method required Build."s);
		}
		return *this;
	}

	ArrayItemContext Builder::StartArray() {
		if (nodes_stack_.empty()) {
			throw std::logic_error("Wrong method. Method required Build."s);
		}

		const auto back_element = nodes_stack_.back();
		if (back_element->IsNull()) {
			*back_element = Array();
			nodes_stack_.emplace_back(nodes_stack_.back());
		}
		else if (back_element->IsArray()) {
			Array& arr = std::get<Array>(back_element->GetValue());
			arr.emplace_back(json::Array());
			nodes_stack_.emplace_back(&arr.back());

		}
		else if (back_element->IsDict()) {
			if (!key_) {
				throw std::logic_error("Attempt to add a value to a map without a key."s);
			}
			Dict& dict = std::get<Dict>(back_element->GetValue());

			const auto dictionary = dict.emplace(*key_, json::Array());
			nodes_stack_.emplace_back(&dictionary.first->second);
			key_ = std::nullopt;
		}
		else {
			throw std::logic_error("Wrong method. Method required Build."s);
		}
		return *this;
	}


	Builder& Builder::EndDict() {
		if (!nodes_stack_.back()->IsDict() || nodes_stack_.empty()) {
			throw std::logic_error("EndDict doesn't close the Dict."s);
		}
		nodes_stack_.pop_back();
		return *this;
	}

	Builder& Builder::EndArray() {
		if (!nodes_stack_.back()->IsArray() || nodes_stack_.empty()) {
			throw std::logic_error("EndArray doesn't close the Array"s);
		}
		nodes_stack_.pop_back();
		return *this;
	}

	Node Builder::Build() {
		if (root_.IsNull()) {
			throw std::logic_error("The object is not build."s);
		}
		if (nodes_stack_.size() > 1 && ((nodes_stack_.back()->IsArray()) || (nodes_stack_.back()->IsDict()))) {
			throw std::logic_error("The object is not build."s);
		}
		return root_;
	}

	// ------ ArrayItemContext ------------

	ArrayItemContext::ArrayItemContext(Builder& builder) : builder_(builder) {}
	ArrayItemContext ArrayItemContext::Value(Node::Value value) {
		builder_.Value(std::move(value));
		return ArrayItemContext{ builder_ };
	}
	DictItemContext ArrayItemContext::StartDict() {
		return builder_.StartDict();
	}
	ArrayItemContext ArrayItemContext::StartArray() {
		return builder_.StartArray();
	}
	Builder& ArrayItemContext::EndArray() {
		return builder_.EndArray();
	}

	// ------ KeyValueContext -------------

	KeyValueContext::KeyValueContext(Builder& builder) : builder_(builder) {}
	DictItemContext KeyValueContext::Value(Node::Value value) {
		builder_.Value(std::move(value));
		return DictItemContext{ builder_ };
	}
	DictItemContext KeyValueContext::StartDict() {
		return builder_.StartDict();
	}
	ArrayItemContext KeyValueContext::StartArray() {
		return builder_.StartArray();
	}

	// ------ DictItemContext -------------

	DictItemContext::DictItemContext(Builder& builder) : builder_(builder) {}
	KeyValueContext DictItemContext::Key(std::string key) {
		return builder_.Key(std::move(key));
	}

	Builder& DictItemContext::EndDict() {
		return builder_.EndDict();
	}
}