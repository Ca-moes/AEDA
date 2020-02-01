#include "Competition.h"
#include "Trial.h"
#include "Date.h"
#include <iomanip>
#include <algorithm>


Competition::Competition(){
    name = "";
    begin = Date();
    end = Date();
}

/**
 * Competition Constructor
 *
 * @param name the name of the Competition
 * @param begin the begin Date of the Competition
 * @param end the begin Date of the Competition
 */
Competition::Competition(string name, Date begin, Date end){
    this->name = name;
    this->begin = begin;
    this ->end = end;
}

Competition::Competition(const Competition & c){
    name = c.getName();
    begin = c.getBegin();
    end = c.getEnd();
    trials = c.getTrials();
    medals = c.getMedals();
    participants =c.getParticipants();
    result = c.getResult();
}

const Date & Competition::getBegin() const{
    return begin;
}

const Date & Competition::getEnd() const{
    return end;
}

const string &  Competition::getName() const{
    return name;
}

const vector<Medal> & Competition::getMedals() const {
    return medals;
}

float Competition::getResult() const {
    return result;
}

void Competition::setResult(float result) {
    this->result = result;
}

void Competition::setName(const string &n) {
    this->name = n;
}

void Competition::setBegin(const Date &b) {
    this->begin = b;
}

void Competition::setEnd(const Date &e) {
    this->end = e;
}

void Competition::setTrials(const vector<Trial> & trials){
    this->trials = trials;
}

void Competition::setParticipants(const vector<string> & p){
    participants=vector<string>(p);
}

const vector<string> & Competition::getParticipants() const{
    return participants;
}

const vector<Trial> & Competition::getTrials() const{
    return trials;
}

void Competition::setMedals(const vector<Medal> &medals){
    vector<Medal> sortedMedals = medals;
    sort(sortedMedals.begin(),sortedMedals.end());
    this->medals = sortedMedals;
}

void Competition::addParticipant(const string & p){
    participants.push_back(p);
}

string Competition::info() const{
    ostringstream os;
    os <<  left <<setw(17) << "Name" << setw(4) << " "<<  getName() << setw(3) <<endl;
    os <<  left <<setw(17) << "Begin Date" << setw(4) << " "<< getBegin() << setw(3) <<endl;
    os <<  left <<setw(17) << "End Date" << setw(4) << " "<< getEnd() << setw(3) <<endl;
    if(!trials.empty()){
        os <<  left <<setw(17) << "Trials" <<setw(4) << endl;
        for(const auto & trial : trials)
            os << trial.info()<<" ";
        os << endl;
    }
    os <<  left <<setw(17) << "Medals" << setw(4) <<endl;
    for(const auto & medal : getMedals())
        os << medal.info()<<" ";
    os << endl;
    return os.str();
}

void Competition::showInfo() const{
    cout <<  left <<setw(17) << "Name" << setw(4) << " "<<  getName() << setw(3) <<endl;
    cout <<  left <<setw(17) << "Begin Date" << setw(4) << " "<< getBegin() << setw(3) <<endl;
    cout <<  left <<setw(17) << "End Date" << setw(4) << " "<< getEnd() << setw(3) <<endl;
    if(!trials.empty()){
        cout <<  left <<setw(17) << "Trials" <<setw(4) << " ";
        for(unsigned int i = 0; i<trials.size();i++){
            cout << trials[i].getName();
            if(i != trials.size()-1)
                cout << ", ";
        }
        cout << endl;
    }
    if(!participants.empty()){
        cout <<  left <<setw(17) << "Participants" <<setw(4) << " ";
        for(unsigned int i = 0; i< participants.size();i++){
            cout << participants[i];
            if(i != participants.size()-1)
                cout << ", ";
        }
        cout << endl;
    }
    cout <<  left <<setw(17) << "Winners" << setw(4) <<endl;
    if(getMedals().empty()) cout << "No Winners/Medals to show - result not available\n";
    else{
        for(const auto & medal : getMedals()){
            if(medal.getType() == 'g')
                cout << left << setw(2) <<left << "->" << left <<setw(15) << "Gold" << setw(4) << " "<< medal.getWinner() << setw(3) <<endl;
            else if(medal.getType() == 's')
                cout <<left << setw(2) << "->" << left <<setw(15) << "Silver" << setw(4) << " "<< medal.getWinner() << setw(3) <<endl;
            else
                cout << left << setw(2) << "->" << left <<setw(15) << "Bronze" << setw(4) << " "<< medal.getWinner() << setw(3) <<endl;
        }
    }
}

void Competition::showMedals() const{
    for(const auto & medal : getMedals()){
        if(medal.getType() == 'g')
            cout << left << setw(2) <<left << "->" << left <<setw(15) << "Gold" << setw(4) << " "<< medal.getWinner() << setw(3) <<endl;
        else if(medal.getType() == 's')
            cout <<left << setw(2) << "->" << left <<setw(15) << "Silver" << setw(4) << " "<< medal.getWinner() << setw(3) <<endl;
        else
            cout << left << setw(2) << "->" << left <<setw(15) << "Bronze" << setw(4) << " "<< medal.getWinner() << setw(3) <<endl;
    }
}

void Competition::showResult() const {
    string tmp = name;
    transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    cout << tmp << endl;
    cout <<  left <<setw(17) << "Date" << setw(4) << " "<< getEnd() << setw(3) <<endl;
    cout <<  left <<setw(17) << "Result" <<setw(4) << " "<< getResult() << endl;
    cout <<  left <<setw(17) << "Winner's Country" << setw(4) << " "<< getMedals()[0].getCountry() <<endl;
    cout <<  left <<setw(17) << "Winner" << setw(4) << " "<< getMedals()[0].getWinner() <<endl;
}

ostream& operator<<(ostream& os, const Competition & c)
{
    os << c.getName() << "\n";
    return os;
}

bool Competition::operator == (const Competition & c2) const{
    return getName() == c2.getName();
}