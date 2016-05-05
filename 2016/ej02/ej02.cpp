#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>

const int MAX_WORDS = 35000;

struct WordCount {
	inline bool operator<(const WordCount & other_word) const {
		return frequency < other_word.frequency;
	}

	inline bool operator>(const WordCount & other_word) const {
		return frequency > other_word.frequency;
	}

	size_t word_id;
	size_t frequency;
};

struct Corpus {
	static Corpus read(const std::string & filename) {
		Corpus corpus;

		std::ifstream file { filename };
		std::string word;
		corpus.document.reserve(MAX_WORDS);
		size_t id;
		while (file >> word) {
			id = corpus.registerWord(word);
			corpus.document.push_back(id);
		}

		return std::move(corpus);
	}

	const std::string & word(size_t word_id) const {
		return words[word_id];
	}

	size_t registerWord(const std::string & word) {
		auto it = word_ids.find(word);
		if (it == word_ids.end()) {
			size_t id = word_ids.size();
			word_ids.insert(std::make_pair(word, id));
			words.push_back(word);
			return id;
		}
		return it->second;
	}

	std::vector<WordCount> topFrequency(size_t begin, size_t end,
			size_t num_words) {
		static std::vector<size_t> word_count(MAX_WORDS);
		size_t * word_ptr = word_count.data();
		std::fill(word_count.begin(), word_count.end(), 0);

		auto document_begin = document.data() + (begin - 1);
		auto document_end = document.data() + end;
		for (; document_begin != document_end; ++document_begin) {
			++word_ptr[*document_begin];
		}

		// find out num_words max
		std::vector<WordCount> max_words;
		max_words.reserve(num_words + 1);
		size_t word_id = 0;
		for (; word_id < word_ids.size() && max_words.size() < num_words;
				++word_id, ++word_ptr) {
			max_words.push_back(WordCount( { word_id, *word_ptr }));
		}

		std::sort(max_words.begin(), max_words.end(),
				std::greater<WordCount>());
		if (word_id < word_ids.size()) {
			max_words.push_back(WordCount( { 0, 0 }));
			for (; word_id < word_ids.size(); ++word_id, ++word_ptr) {
				max_words.back() = WordCount( { word_id, *word_ptr });

				// insert new word count
				auto max_word = max_words.rbegin();
				auto prev_word = max_word + 1;
				auto max_word_end = max_words.rend();
				while (prev_word != max_word_end && *prev_word < *max_word) {
					std::swap(*max_word, *prev_word);
					max_word = prev_word;
					++prev_word;
				}
			}

			max_words.resize(num_words);
		}

		return max_words;
	}

	std::map<std::string, size_t> word_ids;
	std::vector<std::string> words;
	std::vector<size_t> document;
};

int main() {
	size_t num_cases, start_word, end_word;
	std::cin >> num_cases;

	auto corpus = Corpus::read("corpus.txt");
	for (size_t num_case = 1; num_case <= num_cases; ++num_case) {
		std::cin >> start_word >> end_word;
		auto top_freq = corpus.topFrequency(start_word, end_word, 3);
		std::cout << "Case #" << num_case << ": ";
		bool first = true;
		for (auto & freq : top_freq) {
			if (!first) {
				std::cout << ",";
			}
			std::cout << corpus.word(freq.word_id) << " " << freq.frequency;
			first = false;
		}
		std::cout << "\n";
	}
}
