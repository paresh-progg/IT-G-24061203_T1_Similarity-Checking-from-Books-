#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <algorithm>

using namespace std;

vector<string> readWordsFromFile(const string &filename)
{
    ifstream file(filename);
    vector<string> words;
    string word;

    if (file.is_open())
    {
        while (file >> word)
        {
            words.push_back(word);
        }
        file.close();
    }
    else
    {
        cerr << "Could not open file: " << filename << endl;
    }
    return words;
}

map<string, double> calculateWordFrequencies(const vector<string> &words)
{
    map<string, double> frequencyMap;
    int totalWords = words.size();

    for (const string &word : words)
    {
        frequencyMap[word] += 1.0 / totalWords;
    }

    return frequencyMap;
}

// Function to calculate similarity score between two books
double calculateSimilarity(const map<string, double> &freq1, const map<string, double> &freq2)
{
    double similarityScore = 0.0;

    for (const auto &entry : freq1)
    {
        const string &word = entry.first;
        if (freq2.count(word))
        {
            similarityScore += entry.second + freq2.at(word);
        }
    }

    return similarityScore;
}

vector<pair<int, int>> findTopSimilarPairs(const vector<vector<string>> &allBooks, const vector<string> &bookFiles)
{
    int numBooks = allBooks.size();
    vector<map<string, double>> allFrequencies;
    vector<tuple<int, int, double>> allScores;

    for (const auto &words : allBooks)
    {
        allFrequencies.push_back(calculateWordFrequencies(words));
    }

    for (int i = 0; i < numBooks; ++i)
    {
        for (int j = i + 1; j < numBooks; ++j)
        {
            double similarity = calculateSimilarity(allFrequencies[i], allFrequencies[j]);
            allScores.push_back({i, j, similarity});
        }
    }

    sort(allScores.begin(), allScores.end(), [](auto &a, auto &b)
         { return get<2>(a) > get<2>(b); });

    // Display top 10 pairs
    vector<pair<int, int>> topPairs;
    cout << "Top 10 Similar Book Pairs:\n";
    for (int k = 0; k < min(10, (int)allScores.size()); ++k)
    {
        int book1Index = get<0>(allScores[k]);
        int book2Index = get<1>(allScores[k]);
        double score = get<2>(allScores[k]);

        cout << bookFiles[book1Index] << " and " << bookFiles[book2Index]
             << " - Similarity Score: " << score << endl;

        topPairs.push_back({book1Index, book2Index});
    }

    return topPairs;
}

int main()
{
    string fileNamesFile = "book_files.txt"; // File containing list of book filenames
    ifstream file(fileNamesFile);
    if (!file.is_open())
    {
        cerr << "Could not open file: " << fileNamesFile << endl;
        return 1;
    }

    vector<string> bookFiles;
    string filename;
    while (getline(file, filename))
    {
        if (!filename.empty())
        {
            bookFiles.push_back(filename);
        }
    }
    file.close();

    vector<vector<string>> allBooks;
    for (const string &filename : bookFiles)
    {
        allBooks.push_back(readWordsFromFile(filename));
    }

    findTopSimilarPairs(allBooks, bookFiles);

    return 0;
}