#pragma once

#include <vector>
#include <functional>
#include <optional>
#include <string>
#include <sstream>

namespace My {

	template<typename T = int>
	class DoubleLinkedList {
		using index = int;

		struct Node {
			T val;
			Node* next;
			Node* back;
		};

	public:
		DoubleLinkedList() : m_start(nullptr), m_end(nullptr), m_size(0) {}

		DoubleLinkedList(const std::vector<T>& fill);
		DoubleLinkedList(const DoubleLinkedList& d);
		~DoubleLinkedList();
		
		std::string log_nodes() const;
		
		std::optional<std::reference_wrapper<T>> operator[](int x);

		void push(index x, T el);
		void push_back(T el);
		void push_forth(T el);

		void pop(index x);
		void pop_end();
		void pop_begin();
		
		index search_same(T el) const;

		inline unsigned size() { return m_size; }

	private:
		Node* GetNode(int x);

		Node* m_start;
		Node* m_end;
		unsigned m_size;
	};

	template<typename T>
	DoubleLinkedList<T>::DoubleLinkedList(const std::vector<T>& fill) : m_start(nullptr), m_end(nullptr), m_size(0) {
		for (const T& el : fill) {
			push_back(el);
		}
	}

	template<typename T>
	DoubleLinkedList<T>::DoubleLinkedList(const DoubleLinkedList& d) : m_start(nullptr), m_end(nullptr), m_size(0) {
		Node* current = d.m_start;
		for (unsigned i = 0; i < d.m_size; ++i) {
			push_back(current->val);
			current = current->next;
		}
	}

	template<typename T>
	DoubleLinkedList<T>::~DoubleLinkedList() {
		while (m_size > 0) {
			pop_begin();
		}
	}

	template<typename T>
	std::string DoubleLinkedList<T>::log_nodes() const {
		if (m_size == 0) return "List is empty.";
		std::ostringstream oss;
		Node* current = m_start;
		for (unsigned i = 0; i < m_size; ++i) {
			oss << current->val << " ";
			current = current->next;
		}
		return oss.str();
	}

	template<typename T>
	std::optional<std::reference_wrapper<T>> DoubleLinkedList<T>::operator[](int x) {
		if (x < 0 || static_cast<unsigned>(x) >= m_size) {
			return std::nullopt;
		}
		return GetNode(x)->val;
	}

	template<typename T>
	void DoubleLinkedList<T>::push_back(T el) {
		Node* new_node = new Node{ el, nullptr, nullptr };
		if (m_size == 0) {
			m_start = new_node;
			m_end = new_node;
			new_node->next = new_node;
			new_node->back = new_node;
		} else {
			new_node->back = m_end;
			new_node->next = m_start;
			m_end->next = new_node;
			m_start->back = new_node;
			m_end = new_node;
		}
		++m_size;
	}

	template<typename T>
	void DoubleLinkedList<T>::push_forth(T el) {
		push_back(el); 
		m_start = m_end; 
	}

    template<typename T>
	void DoubleLinkedList<T>::push(index x, T el) {
		if (x > m_size) {
			throw std::out_of_range("Index out of bounds");
		}
		if (x == 0) {
			push_forth(el);
			return;
		}
		if (x == m_size) {
			push_back(el);
			return;
		}
		Node* current = GetNode(x);
		Node* new_node = new Node{ el, current, current->back };
		current->back->next = new_node;
		current->back = new_node;

		++m_size;
	}

	template<typename T>
	void DoubleLinkedList<T>::pop(index x) {
		if (x >= m_size) {
			throw std::out_of_range("Index out of bounds");
		}
		if (x == 0) {
			pop_begin();
			return;
		}
		if (x == m_size - 1) {
			pop_end();
			return;
		}
		
		Node* current = GetNode(x);
		current->back->next = current->next;
		current->next->back = current->back;

		delete current;
		--m_size;
	}
	
	template<typename T>
	void DoubleLinkedList<T>::pop_end() {
		if (m_size == 0) return;
		if (m_size == 1) {
			delete m_end;
			m_start = nullptr;
			m_end = nullptr;
		} else {
			Node* ref_node = m_end->back;
			ref_node->next = m_start;
			m_start->back = ref_node;
			delete m_end;
			m_end = ref_node;
		}
		--m_size;
	}

	template<typename T>
	void DoubleLinkedList<T>::pop_begin() {
		if (m_size == 0) return;
		if (m_size == 1) {
			delete m_start;
			m_start = nullptr;
			m_end = nullptr;
		} else {
			Node* ref_node = m_start->next;
			ref_node->back = m_end;
			m_end->next = ref_node;
			delete m_start;
			m_start = ref_node;
		}
		--m_size;
	}
						
	template<typename T>
    DoubleLinkedList<T>::index DoubleLinkedList<T>::search_same(T el) const {
		Node* current = m_start;
		for (unsigned i = 0; i < m_size; ++i) {
			if (current->val == el) {
				return i;
			}
			current = current->next;
		}
		return -1; 
	}

	template<typename T>
	typename DoubleLinkedList<T>::Node* DoubleLinkedList<T>::GetNode(int x) {
		Node* current = m_start;
		for (int i = 0; i < x; ++i) {
			current = current->next;
		}
		return current;
	}

}
