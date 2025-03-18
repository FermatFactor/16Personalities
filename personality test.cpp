#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdlib>
using namespace std;

// ANSI colour codes for terminal output
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

// --- Adaptive Engine for AI-like behavior ---
// Abstract base class for adaptive scoring
class AdaptiveEngine {
public:
    // Virtual function to adjust the raw score for a given question
    virtual int adjustScore(int rawScore, int questionIndex) = 0;
    virtual ~AdaptiveEngine() {}
};

// A simple adaptive engine that slightly modifies score for specific questions
class SimpleAdaptiveEngine : public AdaptiveEngine {
public:
    // For demonstration, if the question index is 0 and user gave a strong positive response,
    // add an extra point. Otherwise, return the raw score.
    int adjustScore(int rawScore, int questionIndex) override {
        if(questionIndex == 0 && rawScore == 2) { // strong positive on question 0
            return rawScore + 1;
        }
        // More rules can be added here if needed.
        return rawScore;
    }
};

// --- Structure for a Test Question ---
// Each question affects one of the four dimensions:
// 0: E/I, 1: S/N, 2: T/F, 3: J/P
// For each dimension, a positive score gives one letter and a negative score gives the other.
struct Question {
    string text;
    int dimension;         // 0 for E/I, 1 for S/N, 2 for T/F, 3 for J/P
    char positiveLetter;   // e.g., 'E'
    char negativeLetter;   // e.g., 'I'
};

// --- Function Declarations ---
void printIntro();
void runTest();
char calculateDimensionLetter(int score, char positive, char negative);
void generateReport(const string &userName, const string &personality, const int scores[4]);
void saveResultToFile(const string &userName, const string &report);

// --- Main Function ---
int main() {
    try {
        bool retake = false;
        do {
            printIntro();
            runTest();
            cout << GREEN << "\nDo you want to retake the test? (y/n): " << RESET;
            char choice;
            cin >> choice;
            retake = (choice == 'y' || choice == 'Y');
            cin.ignore(); // Clear newline from input
            cout << "\n";
        } while(retake);
    }
    catch(const exception &e) {
        cerr << RED << "An error occurred: " << e.what() << RESET << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// --- Function Definitions ---

// Print introductory message with colours
void printIntro() {
    cout << CYAN << "====================================================" << RESET << "\n";
    cout << CYAN << "      Welcome to the 16Personalities Test!         " << RESET << "\n";
    cout << CYAN << "====================================================\n" << RESET;
    cout << YELLOW << "This test will help you discover your personality type\n";
    cout << "Answer each question on a scale from 1 (Strongly Agree) to 5 (Strongly Disagree).\n" << RESET;
}

// Run the personality test
void runTest() {
    // Ask for user name
    string userName;
    cout << GREEN << "\nEnter your name: " << RESET;
    cin >> userName;
    cin.ignore();

    // Define the questions for the test (for simplicity, 8 questions, 2 per dimension)
    // Note: raw score = answer - 3, mapping 1->-2, 3->0, 5->+2
    vector<Question> questions = {
        { "You enjoy large social gatherings.", 0, 'E', 'I' },
        { "You feel recharged after spending time with people.", 0, 'E', 'I' },
        { "You focus on concrete details rather than abstract ideas.", 1, 'S', 'N' },
        { "You trust experiences over theories.", 1, 'S', 'N' },
        { "You rely on logical analysis rather than personal feelings when making decisions.", 2, 'T', 'F' },
        { "You can easily detach yourself from emotional situations.", 2, 'T', 'F' },
        { "You prefer having a structured schedule over being spontaneous.", 3, 'J', 'P' },
        { "You like planning your activities well in advance.", 3, 'J', 'P' }
    };

    // Initialize counters for each personality dimension (0: E/I, 1: S/N, 2: T/F, 3: J/P)
    int scores[4] = {0, 0, 0, 0};

    // Create an instance of the adaptive engine
    SimpleAdaptiveEngine adaptiveEngine;

    // Loop through each question
    for (size_t i = 0; i < questions.size(); i++) {
        cout << "\n" << MAGENTA << "Question " << i + 1 << ": " << RESET;
        cout << questions[i].text << "\n";
        // Display options in colour
        cout << BLUE << "1. Strongly Agree\n2. Agree\n3. Neutral\n4. Disagree\n5. Strongly Disagree\n" << RESET;
        cout << GREEN << "Enter your choice (1-5): " << RESET;
        int choice;
        cin >> choice;
        // Validate input
        if (choice < 1 || choice > 5) {
            throw runtime_error("Invalid input! Please choose a number between 1 and 5.");
        }
        // Convert the answer to a score: (1 -> -2, 2 -> -1, 3 -> 0, 4 -> +1, 5 -> +2)
        int rawScore = choice - 3;
        // Use the adaptive engine to adjust score if necessary
        int adjustedScore = adaptiveEngine.adjustScore(rawScore, i);
        // Update the corresponding dimension's score
        scores[questions[i].dimension] += adjustedScore;
    }

    // Calculate the final personality type (4-letter type)
    string personality = "";
    personality += calculateDimensionLetter(scores[0], 'E', 'I');
    personality += calculateDimensionLetter(scores[1], 'S', 'N');
    personality += calculateDimensionLetter(scores[2], 'T', 'F');
    personality += calculateDimensionLetter(scores[3], 'J', 'P');

    // Generate and display the detailed personality report
    generateReport(userName, personality, scores);
}

// Helper function to choose the personality letter based on the score
char calculateDimensionLetter(int score, char positive, char negative) {
    return (score >= 0) ? positive : negative;
}

// Generate the detailed report and save it to file
void generateReport(const string &userName, const string &personality, const int scores[4]) {
    // For simplicity, we'll generate a report for a couple of known personality types.
    // In a full implementation, you would add detailed descriptions for all 16 types.
    string report = "============================================\n";
    report += "          Personality Test Report           \n";
    report += "============================================\n\n";
    report += "Name: " + userName + "\n";
    report += "Personality Type: " + personality + "\n\n";

    // Append a simple percentage-like score for each dimension
    report += "Trait Scores:\n";
    report += "E/I: " + to_string(scores[0]) + "\n";
    report += "S/N: " + to_string(scores[1]) + "\n";
    report += "T/F: " + to_string(scores[2]) + "\n";
    report += "J/P: " + to_string(scores[3]) + "\n\n";

    // Detailed explanation based on personality type
    if (personality == "ENTJ") {
        report += "Type: ENTJ - The Commander\n";
        report += "Description: Bold, imaginative, and strong-willed leaders, always finding a way – or making one.\n";
        report += "Strengths: Efficient, strategic, self-confident.\n";
        report += "Weaknesses: Stubborn, intolerant, impatient.\n";
        report += "Career Paths: Management, Entrepreneurship, Law.\n";
        report += "Partner Preferences: Someone who is independent and intellectually stimulating.\n";
    }
    else if (personality == "INFP") {
        report += "Type: INFP - The Mediator\n";
        report += "Description: Idealistic, loyal to their values and to people who are important to them.\n";
        report += "Strengths: Empathetic, open-minded, creative.\n";
        report += "Weaknesses: Overly idealistic, self-critical, prone to stress.\n";
        report += "Career Paths: Counseling, Writing, Art.\n";
        report += "Partner Preferences: Understanding, kind, and supportive individuals.\n";
    }
    else {
        report += "Type: " + personality + "\n";
        report += "Description: A unique blend of traits. Explore your strengths, weaknesses, and preferences further.\n";
    }
    
    // Display the report on screen
    cout << "\n" << CYAN << "========== Personality Report ==========" << RESET << "\n";
    cout << report << "\n";

    // Save the report to a file
    saveResultToFile(userName, report);
}

// Save result report to file using file handling
void saveResultToFile(const string &userName, const string &report) {
    string filename = userName + "_personality.txt";
    ofstream outFile(filename);
    if (!outFile) {
        throw runtime_error("Error opening file for writing!");
    }
    outFile << report;
    outFile.close();
    cout << YELLOW << "Your results have been saved to " << filename << RESET << "\n";
}
