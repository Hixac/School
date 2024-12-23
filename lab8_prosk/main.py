class Word:
    def __init__(self, word, pages, count):
        self.__word = word 
        self.__pages = pages 
        self.__count = count 

    def __repr__(self):
        return f"Word(word='{self.__word}', pages={self.__pages}, count={self.__count})"

    def input_fields(self):
        self.__word = input("Введите слово: ")
        self.__pages = list(map(int, input("Введите номера страниц через пробел: ").split()))
        self.__count = int(input("Введите количество повторений: "))

    def display_fields(self):
        print(f"Слово: {self.__word}")
        print(f"Страницы: {self.__pages}")
        print(f"Количество повторений: {self.__count}")

    def is_word_length_valid(self, min_length):
        return len(self.__word) >= min_length

    def is_count_in_range(self, min_count, max_count):
        return min_count <= self.__count <= max_count

    def get_word(self):
        return self.__word

    def get_pages(self):
        return self.__pages

    def get_count(self):
        return self.__count

words = [
    Word("example", [1, 2, 5], 120),
    Word("test", [2, 3], 90),
    Word("python", [3, 4, 7], 150),
    Word("data", [1, 8], 50),
    Word("code", [5, 6], 200)
]

def words_in_page_range(words, start_page, end_page):
    return [word.get_word() for word in words if any(start_page <= page <= end_page for page in word.get_pages())]

def words_with_high_count(words, threshold=100):
    return [word.get_word() for word in words if word.get_count() > threshold]

def pages_for_word(words, target_word):
    for word in words:
        if word.get_word() == target_word:
            return word.get_pages()
    return []

class Ms:
    def __init__(self):
        self.__word_list = []

    def add_word(self, word):
        self.__word_list.append(word)

    def display_all_words(self):
        for word in self.__word_list:
            word.display_fields()

    def filter_words(self, condition):
        return [word for word in self.__word_list if condition(word)]
    
    def save_to_file(self, words, filename):
        with open(filename, "w") as file:
            for word in words:
                pages_str = ' '.join([str(i) for i in word.get_pages()])
                file.write(f"{word.get_word()} {pages_str} {word.get_count()}\n")
    
    def load_from_file(self, filename):
        words = []
        with open(filename, "r") as file:
            for line in file:
                parts = line.strip().split()
                word = parts[0]
                pages = list(map(int, parts[1:]))
                count = int(parts[-1])
                words.append(Word(word, pages, count))
        return words
    

ms = Ms()

words.sort(key=lambda w: w.get_count(), reverse=True)

print("Слова в диапазоне страниц 2-5:", words_in_page_range(words, 2, 5))
print("Слова, повторяющиеся больше 100 раз:", words_with_high_count(words))
print("Страницы для слова 'python':", pages_for_word(words, "python"))
print("Отсортированный список по количеству повторений:", words)

ms.save_to_file(words, "words.txt")
new_words = ms.load_from_file("words.txt")
print("Слова из файла:", new_words)

ms.add_word(Word("example", [1, 2, 5], 120))
ms.add_word(Word("test", [2, 3], 90))
ms.display_all_words()
