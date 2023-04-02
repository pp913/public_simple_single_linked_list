#pragma once
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <initializer_list>

//SingleLinkedList
template <typename Type>
class SingleLinkedList {
public:

    //Конструкторы
    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        Clear();
        auto it = values.end()-1;
        while (it != values.begin()) {
            this->PushFront(*it); 
            --it;
            }
        this->PushFront(*values.begin()); 
    }    
        

    SingleLinkedList(const SingleLinkedList& other) {    
        // Сначала надо удостовериться, что текущий список пуст
        assert(size_ == 0 && head_.next_node == nullptr);
        SingleLinkedList tmp;
        //скопировать внутрь tmp элементы other 
        auto it = other.begin();
         while (it != other.end()) {
            tmp.PushFront(*it);
            it++;
           }
        SingleLinkedList tmp2;
          auto it2 = tmp.begin();
         while (it2 != tmp.end()) {
            tmp2.PushFront(*it2);
            it2++;
           }
        // После того как элементы скопированы, обмениваем данные текущего списка и tmp
        swap(tmp2);
        // Теперь tmp пуст, а текущий список содержит копию элементов other
    }

    ~SingleLinkedList(){
        Clear();
    }

    //Оператор копирующего присваивания
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
    SingleLinkedList tmp;
        auto it = rhs.begin();
        while (it != rhs.end()) {
            tmp.PushFront(*it);
            it++;  
        }
        SingleLinkedList tmp2;
        auto it2 = tmp.begin();
        while (it2 != tmp.end()) {
            tmp2.PushFront(*it2);
            it2++;
        }
        swap(tmp2);
        return *this;
    }


    
    
    // Структура - узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value = Type();
        Node* next_node = nullptr;
    };

    //BasicIterator - шаблонный класс, на основе которого объявляются итераторы списка для поддержки перебора
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node *node) 
                :node_(node)
        {
        }
        public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора
        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept 
           :node_(other.node_) {}

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявляется оператор = 
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;   
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
            assert(node_ != nullptr);
            BasicIterator it(node_);
            node_ = node_->next_node;
            return it;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
            
        }
        private:
        Node* node_ = nullptr;
    }; //BasicIterator.end
    
    //Определения
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{ head_.next_node };
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        Node *ptr = head_.next_node;
        if(!ptr) return Iterator{ ptr };
        while(ptr != nullptr){
        ptr = ptr->next_node;
        }
        return Iterator{ ptr };
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{ head_.next_node };
        
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        Node *ptr = head_.next_node;
        if(!ptr) return Iterator{ ptr };
        while(ptr != nullptr){
        ptr = ptr->next_node;
        }
        return ConstIterator{ ptr };
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{ head_.next_node };
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        Node *ptr = head_.next_node;
        if(!ptr) return Iterator{ ptr };
        while(ptr != nullptr){
        ptr = ptr->next_node;
        }
        return ConstIterator{ ptr };
    }


    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
                return Iterator{ &head_ };
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
                return ConstIterator{ const_cast<Node*>(&head_) };
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
                return ConstIterator{ &head_ };
    }
    
    
    //Методы списка
    
    //Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        using std::swap;
        swap(head_.next_node, other.head_.next_node);
        swap(size_, other.size_);
    }
    
    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
    
    //Очистка списка
    void Clear() noexcept {
       if (size_ == 0 && head_.next_node == nullptr) {
            return;
        }
        while (head_.next_node) {
            auto next = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = next;
        }
        head_.next_node = nullptr;
        size_ = 0;
    }


    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }


    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return (size_ == 0);
    }
    
    // Вставляет элемент value после элемента, на который указывает pos.
    // Возвращает итератор на вставленный элемент
    // Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);
        pos.node_->next_node = new Node (value, pos.node_->next_node);
        size_++;
        return Iterator {pos.node_->next_node};
    }


    // Удаляет первый элемента непустого списка за время O(1).
    void PopFront() noexcept {
        assert(!(size_ == 0 && head_.next_node == nullptr));
        auto next = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = next;
        size_--;
    }

   
    // Удаляет элемент, следующий за pos.
    // Возвращает итератор на элемент, следующий за удалённым
    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr);
        auto next = pos.node_->next_node->next_node;
        delete pos.node_->next_node;  
        pos.node_->next_node = next;
        size_--;
        return Iterator {next};
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_ = {Type(), nullptr};
    size_t size_ = 0;
}; //SingleLinkedList.end



//Операторы
template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()) && lhs.GetSize() == rhs.GetSize());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
     return !(std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()) && lhs.GetSize() == rhs.GetSize());
}

template <typename Type> //
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                             rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(std::lexicographical_compare(lhs.begin(), lhs.end(),
                             rhs.begin(), rhs.end()));
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(std::lexicographical_compare(lhs.begin(), lhs.end(),
                             rhs.begin(), rhs.end()));
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(std::lexicographical_compare(lhs.begin(), lhs.end(),
                             rhs.begin(), rhs.end()));
} 






