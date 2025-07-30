
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <exception>
#include <chrono>
#include <sstream>
#include <functional>
#include <numeric>
#include <cstdlib>  
#include <ctime>    






#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

using namespace std;
class InputException : public std::exception

 {
    string message;
public:
    InputException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};


template<typename T>
T getValidatedInput()
 {
    T value;
    while (true) {
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << RED << "Invalid input. Please try again: " << RESET;
        } else {
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}


struct MBTIResult
 {
    int E = 0, I = 0;
    int S = 0, N = 0;
    int T = 0, F = 0;
    int J = 0, P = 0;
};

struct Personality
 {
    string name;
    string description;
    vector<std::string> strengths;
    vector<std::string> weaknesses;
    vector<std::string> careers;
};


class Question 
{
protected:
    string prompt;
public:
    Question(const std::string& q) : prompt(q) {}
    virtual void ask(MBTIResult& result) = 0;
    virtual ~Question() {}
};


class ChoiceQuestion : public Question {
    char dimension;
    char positiveTrait;
    char negativeTrait;
public:
    ChoiceQuestion(const std::string& q, char dim, char pos, char neg)
        : Question(q), dimension(dim), positiveTrait(pos), negativeTrait(neg) {}

    void ask(MBTIResult& result) override {
        cout << CYAN << prompt << RESET << "\n";
        cout << "  1. Agree (" << positiveTrait << ")\n";
        cout << "  2. Disagree (" << negativeTrait << ")\n";
        cout << YELLOW << "Your choice (1-2): " << RESET;
        int choice = getValidatedInput<int>();
        if (choice == 1) incrementTrait(result, positiveTrait);
        else if (choice == 2) incrementTrait(result, negativeTrait);
        else throw InputException("Choice must be 1 or 2.");
    }

    void incrementTrait(MBTIResult& res, char trait) {
        switch (trait) {
            case 'E': res.E++; break;
            case 'I': res.I++; break;
            case 'S': res.S++; break;
            case 'N': res.N++; break;
            case 'T': res.T++; break;
            case 'F': res.F++; break;
            case 'J': res.J++; break;
            case 'P': res.P++; break;
            default: break;
        }
    }
};


class ProgressInterface {
public:
    virtual void showProgress(size_t current, size_t total) = 0;
    virtual ~ProgressInterface() {}
};

class ASCIIArtInterface {
public:
    virtual void displayChart(const MBTIResult& result) = 0;
    virtual ~ASCIIArtInterface() {}
};


class ProgressBar : virtual public ProgressInterface 
{
public:
    void showProgress(size_t current, size_t total) override {
        const int width = 50;
        float ratio = float(current) / float(total);
        int pos = int(ratio * width);
        cout << BLUE << "[";
        for (int i = 0; i < width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        cout << "] " << int(ratio * 100.0) << " %\r" << RESET;
        cout.flush();
    }
};

class ASCIIChart : virtual public ASCIIArtInterface
 {
public:
    void displayChart(const MBTIResult& r) override {
        map<char,int> scores = {
            {'E', r.E}, {'I', r.I},
            {'S', r.S}, {'N', r.N},
            {'T', r.T}, {'F', r.F},
            {'J', r.J}, {'P', r.P}
        };
        cout << MAGENTA << "\nPersonality Scores Chart:\n" << RESET;
        for (auto& kv : scores) {
            cout << kv.first << " : ";
            for (int i = 0; i < kv.second; ++i) std::cout << "#";
            cout << " (" << kv.second << ")\n";
        }
        cout << "\n";
    }
};


class UXFeatures : public ProgressBar, public ASCIIChart {};


void loadInitialQuestions(std::vector<Question*>& questions);
void loadAdditionalQuestions(std::vector<Question*>& questions);
void loadPersonalityDB(std::map<std::string, Personality>& db);


template<typename Container>
void writeList(std::ofstream& out, const std::string& title, const Container& c)
 {
    out << title << "\n";
    for (const auto& item : c) out << "- " << item << "\n";
    out << "\n";
}


class HistoryLogger
 {
    string filename;
public:
    HistoryLogger(const std::string& fname) : filename(fname) {}
    void log(const std::string& userName, const std::string& type) {
        ofstream hfile(filename, std::ios::app);
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        hfile << userName << " : " << type << " @ " << std::ctime(&now);
        hfile.close();
    }
};


map<std::string,std::vector<std::string>> compatibility = {
    {"INTJ", {"ENFP","ENTP","INFP"}},
    {"ENTP", {"INFJ","INTJ","ENFJ"}},
    {"ISFJ", {"ESFP","ESTP","ESFJ"}},
    {"ESTJ", {"INFP","ISFP","ISTP"}},
    {"INFP", {"ENFJ","ENTJ","ESFJ"}},
    {"ENFJ", {"INFP","ISFP","ISTP"}},
    {"ISTJ", {"ESFP","ESTP","ESFJ"}},
    {"ENFP", {"INTJ","INFJ","ENTJ"}},
    {"ISFP", {"ESTJ","ESFJ","ENTJ"}},
    {"ISTP", {"ESFJ","ESTJ","ENFJ"}},
    {"ESFJ", {"INTJ","ISTP","ISFP"}},
    {"ESTP", {"INFJ","INTJ","ISFJ"}},
    {"INFJ", {"ENFP","ENTP","ESFP"}},
    {"ESFP", {"ISTJ","ISFJ","ESTJ"}},
    {"ENTJ", {"INFP","ISFP","INTP"}},
    {"INTP", {"ENFJ","ENTJ","ENTP"}}
};


void loadInitialQuestions(std::vector<Question*>& questions) {
    questions.reserve(80);
    questions.push_back(new ChoiceQuestion("You enjoy vibrant social events with lots of people.", 'E', 'E', 'I'));
    questions.push_back(new ChoiceQuestion("You often think about unrealistic yet intriguing ideas.", 'N', 'N', 'S'));
    questions.push_back(new ChoiceQuestion("You prefer to focus on details rather than the big picture.", 'S', 'S', 'N'));
    questions.push_back(new ChoiceQuestion("You make decisions based on logic more than emotions.", 'T', 'T', 'F'));
    // Added questions
    questions.push_back(new ChoiceQuestion("You find it easy to introduce yourself to new people.", 'E', 'E', 'I'));
    questions.push_back(new ChoiceQuestion("You are more interested in facts than theories.", 'S', 'S', 'N'));
    questions.push_back(new ChoiceQuestion("You value justice over mercy.", 'T', 'T', 'F'));
    questions.push_back(new ChoiceQuestion("You like to have a detailed plan rather than leaving things open-ended.", 'J', 'J', 'P'));
    questions.push_back(new ChoiceQuestion("You prefer to improvise rather than spend time coming up with a detailed plan.", 'P', 'P', 'J'));
    questions.push_back(new ChoiceQuestion("You are energized by spending time alone.", 'I', 'I', 'E'));
    questions.push_back(new ChoiceQuestion("You trust reason more than feelings.", 'T', 'T', 'F'));
    questions.push_back(new ChoiceQuestion("You are drawn to abstract concepts more than concrete details.", 'N', 'N', 'S'));
    questions.push_back(new ChoiceQuestion("You like to finish one project before starting another.", 'J', 'J', 'P'));
    questions.push_back(new ChoiceQuestion("You often rely on your experience rather than theoretical alternatives.", 'S', 'S', 'N'));
    
}

struct UserProfile {
    string userName;
    string email;
    int age;
    string savedFile;
};

class ProfileManager 
{

public:
    static UserProfile createProfile() {
        UserProfile profile;
        cout << MAGENTA << "Enter your name: " << RESET;
        getline(std::cin, profile.userName);

        cout << MAGENTA << "Enter your email: " << RESET;
        getline(std::cin, profile.email);

        cout << MAGENTA << "Enter your age: " << RESET;
        profile.age = getValidatedInput<int>();

        profile.savedFile = "MBTI_Detailed_" + profile.userName + ".txt";
        return profile;
    }

    static void saveProfile(const UserProfile& profile) {
        ofstream pf("profiles.txt", std::ios::app);
        pf << profile.userName << "," << profile.email << "," << profile.age << "," << profile.savedFile << "\n";
        pf.close();
    }
};


void loadAdditionalQuestions(std::vector<Question*>& questions)
 {
    questions.push_back(new ChoiceQuestion("You feel energized after engaging in deep conversation.", 'E', 'E', 'I'));
    questions.push_back(new ChoiceQuestion("You prefer to work in a structured environment.", 'J', 'J', 'P'));
    questions.push_back(new ChoiceQuestion("You rely on your gut feelings when making decisions.", 'F', 'F', 'T'));
    questions.push_back(new ChoiceQuestion("You enjoy tackling complex theoretical problems.", 'N', 'N', 'S'));
    questions.push_back(new ChoiceQuestion("You like keeping track of details in your daily tasks.", 'S', 'S', 'N'));
    questions.push_back(new ChoiceQuestion("You often consider multiple viewpoints before concluding.", 'N', 'N', 'S'));
    questions.push_back(new ChoiceQuestion("You prefer concrete facts over abstract ideas.", 'S', 'S', 'N'));
    questions.push_back(new ChoiceQuestion("You act on hunches and intuitions.", 'N', 'N', 'S'));
    questions.push_back(new ChoiceQuestion("You enjoy spontaneous adventures.", 'P', 'P', 'J'));
    questions.push_back(new ChoiceQuestion("You find it challenging to introduce yourself to strangers.", 'I', 'I', 'E'));
    // Added questions
    questions.push_back(new ChoiceQuestion("You are more comfortable following a schedule than being spontaneous.", 'J', 'J', 'P'));
    questions.push_back(new ChoiceQuestion("You often get so lost in thoughts that you ignore or forget your surroundings.", 'N', 'N', 'S'));
    questions.push_back(new ChoiceQuestion("You prefer to work alone rather than in a team.", 'I', 'I', 'E'));
    questions.push_back(new ChoiceQuestion("You are more practical than creative.", 'S', 'S', 'N'));
    questions.push_back(new ChoiceQuestion("You find it easy to empathize with others.", 'F', 'F', 'T'));
    questions.push_back(new ChoiceQuestion("You are more likely to trust your head than your heart.", 'T', 'T', 'F'));
    questions.push_back(new ChoiceQuestion("You like to keep your options open.", 'P', 'P', 'J'));
    questions.push_back(new ChoiceQuestion("You enjoy being the center of attention.", 'E', 'E', 'I'));
    questions.push_back(new ChoiceQuestion("You are more interested in what is possible than what is real.", 'N', 'N', 'S'));
    questions.push_back(new ChoiceQuestion("You prefer to make decisions quickly rather than take your time.", 'J', 'J', 'P'));
    
}


void loadPersonalityDB(std::map<std::string, Personality>& db) {
    db["ESTJ"] = {"Executive", "Organized, group-oriented, keen on tradition and order.",
        {"Efficient","Dedicated","Strong-willed","Integrity-driven","Loyal"},
        {"Inflexible","Judgmental","Uncomfortable with unstructured situations","Stubborn"},
        {"Manager","Administrator","Project Lead","Judge","Military Officer"}
    };
    db["INFP"] = {"Mediator", "Idealistic, curious, seeks meaning and connection.",
        {"Empathetic","Creative","Passionate","Open-minded","Altruistic"},
        {"Overly idealistic","Impractical","Avoids conflict","Self-critical","Easily stressed"},
        {"Writer","Counselor","Psychologist","Artist","Philosopher"}
    };
    db["INTJ"] = {"Architect", "Strategic thinkers, imaginative, plan for the future.",
        {"Analytical","Independent","Decisive","Determined","High self-confidence"},
        {"Arrogant","Judgmental","Overly analytical","Loathe deviance from plan","Insensitive"},
        {"Scientist","Engineer","Judge","Professor","Strategist"}
    };
    db["ENFP"] = {"Campaigner", "Enthusiastic, creative, sociable free spirits.",
        {"Curious","Observant","Energetic","Good Communicator","Excellent People Skills"},
        {"Overthinker","Highly emotional","Easily stressed","Disorganized"},
        {"Psychologist","Journalist","Actor","Consultant","Entrepreneur"}
    };
    
}


int main() {
    
    srand(unsigned(std::time(nullptr)));

    
    UserProfile profile = ProfileManager::createProfile();
    ProfileManager::saveProfile(profile);

    
    std::vector<Question*> questions;
    loadInitialQuestions(questions);
    loadAdditionalQuestions(questions);

    
    MBTIResult result;
    UXFeatures ux;

    size_t total = questions.size();
    std::cout << MAGENTA
              << "\nStarting MBTI Test for " << profile.userName << "...\n"
              << RESET;

    
    for (size_t i = 0; i < total; ++i) {
        ux.showProgress(i, total);

        
    

        std::cout << "\n" << GREEN
                  << "Question " << (i + 1)
                  << " of " << total << ":"
                  << RESET << "\n";
        try {
            questions[i]->ask(result);
        } catch (InputException& e) {
            std::cout << RED << e.what() << RESET << "\n";
            --i;  
        }
        std::cout << "\n";
    }
    ux.showProgress(total, total);
    std::cout << "\n\n";

    
    std::string type;
    type += (result.E >= result.I) ? 'E' : 'I';
    type += (result.S >= result.N) ? 'S' : 'N';
    type += (result.T >= result.F) ? 'T' : 'F';
    type += (result.J >= result.P) ? 'J' : 'P';

    
    std::map<std::string, Personality> db;
    loadPersonalityDB(db);
    Personality p = db[type];

    ux.displayChart(result);
    cout << GREEN
              << profile.userName << ", your MBTI type is: "
              << type << " — " << p.name
              << RESET << "\n\n";
    cout << p.description << "\n\n";

    std::cout << BLUE << "Strengths:" << RESET << "\n";
    for (const auto& s : p.strengths) std::cout << " - " << s << "\n";
    std::cout << BLUE << "\nWeaknesses:" << RESET << "\n";
    for (const auto& w : p.weaknesses) std::cout << " - " << w << "\n";
    std::cout << BLUE << "\nIdeal Careers:" << RESET << "\n";
    for (const auto& c : p.careers) std::cout << " - " << c << "\n";

    
    std::cout << CYAN << "\nCompatible Types:" << RESET << "\n";
    if (compatibility.count(type)) {
        for (auto& ct : compatibility[type])
            std::cout << " - " << ct << "\n";
    } else {
        std::cout << "No data available.\n";
    }
    std::cout << "\n";

    
    HistoryLogger logger("MBTI_History.log");
    logger.log(profile.userName, type);

    
    std::ofstream out(profile.savedFile);
    out << "Name: " << profile.userName << "\n";
    out << "Email: " << profile.email << "\n";
    out << "Age: " << profile.age << "\n";
    out << "MBTI Type: " << type << " — " << p.name << "\n\n";
    out << p.description << "\n\n";
    writeList(out, "Strengths:", p.strengths);
    writeList(out, "Weaknesses:", p.weaknesses);
    writeList(out, "Ideal Careers:", p.careers);
    out << "Compatible Types:\n";
    if (compatibility.count(type)) {
        for (auto& ct : compatibility[type])
            out << "- " << ct << "\n";
    }
    out.close();

    std::cout << YELLOW
              << "\nAll details saved to " << profile.savedFile
              << RESET << "\n";

    
    for (auto q : questions) delete q;
    return 0;
}
