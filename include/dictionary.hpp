#pragma once

#include "word.hpp"

// A dictionnary is a collection of words, which can be saved to and loaded from a file
class Dictionary{
public:
  // Create empty dictionnary
  Dictionary();
  // Load dictionnary from file
  Dictionary(std::string path);

  std::vector<Word> words;

  // Adds a list of words to the dictionnary
  void AddWords(std::vector<Word> words);
  // Saves this dictionnary to a file
  void SaveToFile(std::string path);
};
