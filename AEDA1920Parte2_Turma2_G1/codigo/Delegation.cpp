//
// Created by Diana Freitas on 20/10/2019.
//

#include "Delegation.h"
#include "auxiliar.h"
#include "Menus.h"
#include <regex>
#include <sstream>
#include <utility>
#include <algorithm>
#include <functional>
#include <unistd.h>

Delegation::Delegation() : records(Record()) {
    try {
        readDelegationFile();
    }
    catch (FileError &e) {
        cout << e << endl;
        throw;
    }
    catch (FileStructureError &s) {
        cout << s << endl;
        throw;
    }

    mainMenu(*this);
}

Delegation::~Delegation(){
    //Ir a cada um dos nomes de ficheiros em delegation.txt
    // ficheiro a ficheiro escrever para lá os conteudos guadados nos objectos
    // Para fase inicial dar o nome "<nome_ficheiro>_write.txt" aos ficheiros para não dar overwrite"
    writePeopleFile();
    writeCompetitionsFile();
    writeTeamsFile();
    writeDelegationFile();
    writeRecordsFile();
}

//Reading files functions
void Delegation::readDelegationFile() {
    int numline = 0;
    string line, file;
    ifstream delegationFile;
    do{
        cout << "Delegation .txt File: ";
        cin >> file;
        if(cin.fail())
            cin.clear();
    }while(cin.fail());
    delegationFilename = file;
    file += ".txt";
    delegationFile.open(file);
    if (delegationFile.fail())
        throw FileError(file);

    while (getline(delegationFile, line)) {
        numline++;
        switch (numline) {
            case 1:
                line = regex_replace(line, regex("^ +| +$|( ) +"), "$1"); // remove espaços no fim, inicio e meio
                if (checkStringInput(line) != 0)
                    throw FileStructureError(file);
                setCountry(line);
                break;
            case 2:
                line = regex_replace(line, regex("^ +| +$|( ) +"), "$1");
                if (checkFloatInput(line) != 0)
                    throw FileStructureError(file);
                setDailyCostAthlete(stof(line));
                break;
            case 3:
                line = regex_replace(line, regex("^ +| +$|( ) +"), "$1");
                if (checkFloatInput(line) != 0)
                    throw FileStructureError(file);
                setDailyCostStaff(stof(line));
                break;
            case 4:
                line = regex_replace(line, regex("^ +| +$|( ) +"), "$1");
                if (checkFloatInput(line) != 0)
                    throw FileStructureError(file);
                setTotalCost(stof(line));
                break;
            case 5:
                peopleFilename = regex_replace(line, regex("^ +| +$|( ) +"), "$1");
                if (checkStringInput(line) != 0){
                    throw FileStructureError(file);
                }

                break;
            case 6:
                teamsFilename = regex_replace(line, regex("^ +| +$|( ) +"), "$1");
                if (checkStringInput(line) != 0)
                    throw FileStructureError(file);
                break;
            case 7:
                competitionsFilename = regex_replace(line, regex("^ +| +$|( ) +"), "$1");
                if (checkStringInput(line) != 0)
                    throw FileStructureError(file);
                break;
            case 8:
                recordsFilename = regex_replace(line, regex("^ +| +$|( ) +"), "$1");
                if (checkStringInput(line) != 0)
                    throw FileStructureError(file);
                break;
            default:
                throw FileStructureError(file);
        }
    }
    delegationFile.close();
    delegationFile.clear();
    //Read people file
    delegationFile.open(peopleFilename + ".txt");
    if (delegationFile.fail())
        throw FileError(peopleFilename + ".txt");
    readPeopleFile(fileToLineVector(delegationFile));
    delegationFile.close();
    delegationFile.clear();

    calculateTotalCost();
    sort(people.begin(), people.end(), sortPersons);
    sort(athletes.begin(), athletes.end(), sortMembersAlphabetically<Athlete>);

    //Read teams file

    delegationFile.open(teamsFilename + ".txt");
    if (delegationFile.fail())
        throw FileError(teamsFilename + ".txt");
    readTeamsFile(fileToLineVector(delegationFile));
    delegationFile.close();
    delegationFile.clear();

    //Read competitions file
    delegationFile.open(competitionsFilename + ".txt");
    if (delegationFile.fail()) {
        throw FileError(competitionsFilename + ".txt");
    }
    readCompetitionsFile(fileToLineVector(delegationFile));
    delegationFile.clear();
    delegationFile.close();

    //Read records file
    delegationFile.open(recordsFilename + ".txt");
    if (delegationFile.fail()) {
        throw FileError(recordsFilename + ".txt");
    }
    readRecordsFile(fileToLineVector(delegationFile));
    delegationFile.clear();
    delegationFile.close();

    //set team competitions
    for (auto &team: teams) {//corre o vetor de equipas
        vector<Athlete*> members = team->getAthletes();//para cada equipa guarda o vetor de membros
        vector<string> comps;//para cada equipa, serve para guarda as competições onde participa
        for(auto & member: members){//corre o vetor de membros de uma equipa
            for(auto & athlete : athletes){//corre o vetor de atletas da delegação
                if(*athlete == *member) { //se encontrar o atleta nos atletas
                    vector<string> appendComps = athlete->getCompetitions(); // guarda o vetor de competições
                    comps.insert(comps.end(),appendComps.begin(), appendComps.end()); // adiciona o vetor de competições às competições
                    break;
                }
            }
        }
        noRepeatVector(comps);
        team->setCompetitions(comps);
        comps.resize(0);
        members.resize(0);
    }

    //set competition participants
    vector<Sport*>::iterator it;
    for (it=sports.begin(); it!=sports.end();it++) { //corre todos os desportos
        vector<Competition> sportComps= (*it)->getCompetitions();
        vector<Competition> competitionsToSet;
        for(auto &sportComp: sportComps){
            vector<string> participants;
            if((*it)->isTeamSport()){
                for(auto &team: teams){
                    vector<string> teamComps = team->getCompetitions();
                    for(auto &teamComp: teamComps){
                        if(teamComp == sportComp.getName())
                            participants.push_back(team->getName());
                    }
                }
            }
            else{
                for(auto &athlete: athletes){
                    vector<string> indComps = athlete->getCompetitions();
                    for(auto &indComp: indComps){
                        if(indComp == sportComp.getName())
                            participants.push_back(athlete->getName());
                    }
                }

            }
            noRepeatVector(participants);
            sportComp.setParticipants(participants);
            competitionsToSet.push_back(Competition(sportComp));
        }
        (*it)->setCompetitions(competitionsToSet);
    }

    setRecords();
}

void Delegation::readPeopleFile(const vector<string> &lines) {
    int numline = 0;
    string line;
    Date d;
    bool readFunc = false;
    Athlete *a = nullptr;
    Staff *s = nullptr;
    pair<staffHtabit ,bool> testinsert;
    //Variables to read Competitions:
    istringstream competitionsStream;
    string compStr;
    vector<string> competitions;

    for (size_t i = 0; i < lines.size(); i++) {
        numline++;
        line = lines[i];

        if (line.empty()) { // Se alinha está vazia voltamos a colocar o numLines a 0 para ler a próxima pessoa
            numline = 1;
            i++;
            line = lines[i];
        }

        if (numline == 1) { // se for a primeira linha de uma pessoa vamos ver se é funcionário ou atleta
            readFunc = lines[i + 8].empty();
            competitions.resize(0);
            a = new Athlete();
            s = new Staff();
            competitionsStream.clear();
        }

        //ler atleta ou funcionario
        if (!readFunc) { // se tiver mais de 8 linha estamos perante um atleta
            //ler atleta
            switch (numline) {
                case 1:
                    if (checkStringInput(line) != 0) {
                        throw FileStructureError(peopleFilename);
                    }
                    a->setName(line);
                    break;
                case 2:
                    if (checkDateInput(line, d) != 0) {
                        throw FileStructureError(peopleFilename);
                    }
                    a->setBirth(d);
                    break;
                case 3:
                    if (checkAlphaNumericInput(line) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    a->setPassport(line);
                    break;
                case 4:
                    if (checkDateInput(line, d) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    if (d.isOlimpianDate()) {
                        a->setArrival(d);
                    } else {
                        throw FileStructureError(peopleFilename);
                    }
                    break;
                case 5:
                    if (checkDateInput(line, d) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    if (d.isOlimpianDate()) {
                        a->setDeparture(d);
                    } else {
                        throw FileStructureError(peopleFilename);
                    }
                    break;
                case 6:
                    if (checkStringInput(line) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    a->setSport(line);
                    break;
                case 7:
                    //ler competições - confirmar estrutura
                    competitionsStream.str(line);
                    while (getline(competitionsStream, compStr, '/')) {
                        compStr = regex_replace(compStr, regex("^ +| +$|( ) +"), "$1");
                        if (compStr.empty()){
                            throw FileStructureError(peopleFilename);
                        }
                        competitions.push_back(compStr);
                    }
                    a->setCompetitions(competitions);
                    break;
                case 8:
                    if (checkFloatInput(line) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    a->setWeight(stof(line));
                    break;
                case 9:
                    if (checkFloatInput(line) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    a->setHeight(stof(line));
                    people.push_back(new Athlete(*a));
                    athletes.push_back(new Athlete(*a));
                    break;
                default:
                    throw FileStructureError(peopleFilename);
            }
        }
        else {
            //ler funcionario
            Staff* temp = nullptr;
            switch (numline) {
                case 1:
                    if (checkStringInput(line) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    s->setName(line);
                    break;
                case 2:
                    if (checkDateInput(line, d) != 0) {
                        throw FileStructureError(peopleFilename);
                    }
                    s->setBirth(d);
                    break;
                case 3:
                    if (checkAlphaNumericInput(line) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    s->setPassport(line);
                    break;
                case 4:
                    if (checkDateInput(line, d) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    if (d.isOlimpianDate()) {
                        s->setArrival(d);
                    } else {
                        throw FileStructureError(peopleFilename);
                    }
                    break;
                case 5:
                    if (checkDateInput(line, d) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    if (d.isOlimpianDate()) {
                        s->setDeparture(d);
                    } else {
                        throw FileStructureError(peopleFilename);
                    }
                    break;
                case 6:
                    if (checkStringInput(line) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    s->setFunction(line);
                    break;
                case 7:
                    if (checkPositiveIntInput(line) != 0){
                        cout << "here" << endl;
                        throw FileStructureError(peopleFilename);
                    }
                    s->setEmployed(to_bool(line));
                    break;
                case 8:
                    if (checkFloatInput(line) != 0){
                        throw FileStructureError(peopleFilename);
                    }
                    s->setAvailability(stof(line));
                    temp = new Staff(*s);
                    people.push_back(temp);
                    staff.insert(temp);
                    if(temp->getEmployed()) staffService.push(temp);
                    break;
                default:
                    throw FileStructureError(peopleFilename);
            }
        }
    }
}

void Delegation::writePeopleFile(){
    ofstream myfile (peopleFilename + ".txt");
    if (myfile.is_open())
    {
        for (unsigned int i = 0; i<people.size(); ++i) {
            myfile << people.at(i)->getName() << endl << people.at(i)->getBirth() << endl << people.at(i)->getPassport() << endl << people.at(i)->getArrival() << endl << people.at(i)->getDeparture() << endl;
            if(people.at(i)->isAthlete()){
                Athlete* a = dynamic_cast<Athlete *> (people.at(i));
                myfile << a->getSport() << endl;
                vector<string> comps = a->getCompetitions();
                for (size_t j=0; j < comps.size();j++){
                    myfile << comps[j];
                    if(j<(comps.size()-1)) myfile <<"/";
                }
                myfile << endl << a->getWeight() << endl << a->getHeight();
            }
            else{
                Staff* a = dynamic_cast<Staff *> (people.at(i));
                myfile << a->getFunction() << endl;
                myfile << a->getEmployed() << endl;
                myfile << a->getAvailability();
            }
            if (i != people.size()-1)
                myfile << endl << endl;
        }
        myfile.close();
    }
    else cerr << "Unable to open file";
}

void Delegation::readCompetitionsFile(const vector<string> &lines) {
    int numline = 0;
    string line;
    Date d;
    char read = 's'; // auxiliar para saber se vamos ler um sport, uma competition ou um trial (s,c ou t)
    //objects to create a sport
    bool isTeamSport = false;
    TeamSport *teamSport = nullptr;
    IndividualSport *individualSport = nullptr;
    string name, participant, pCountry;
    Competition competition;
    vector<Competition> competitions;
    istringstream participantsStream;
    vector<Medal> medals;
    Medal medal;
    int medalCount = 0;
    Trial trial;
    vector<string> trialPlayers,trialCountries;
    vector<Trial> trials;
    for (size_t i = 0; i < lines.size() + 1; i++) {
        if (i != lines.size()){
            line = lines[i];
        }
        numline++;
        if (numline == 1) {// se for a primeira linha de uma pessoa vamos ver se é uma nova modalidade, competição ou jogo
            if (line.empty()) {// Se a linha está vazia vamos ler a próxima competição
                if (read == 'c' || read == 't') {
                    if (read == 't') {
                        competition.setTrials(trials);
                    }
                    competition.setMedals(medals);
                    competitions.push_back(competition);
                    medals.resize(0);
                    trials.resize(0);
                }
                competition = Competition();
                trial = Trial();
                medal = Medal();
                read = 'c';
                i++;
                numline = 1;
                line = lines[i];
            } else if (line == "////////" || i == lines.size()) {//novo desporto - guardar os dados das competições e jogos e limpar variáveis auxiliares; ou útlima linha do ficheiro
                if (read == 't' || read == 'c'){
                    if (read == 't') {
                        competition.setTrials(trials);
                    }
                    competition.setMedals(medals);
                    competitions.push_back(competition);
                }
                if (isTeamSport){
                    teamSport->setCompetitions(competitions);
                    for (auto &team : teams) {
                        if (team->getSport() == teamSport->getName())
                            teamSport->addTeam(team);
                    }
                    sports.push_back(new TeamSport(*teamSport));
                } else {
                    individualSport->setCompetitions(competitions);
                    for (auto &athlete : athletes) {
                        if (athlete->getSport() == individualSport->getName())
                            individualSport->addAthlete(athlete);
                    }
                    sports.push_back(new IndividualSport(*individualSport));
                }
                competitions.resize(0);
                trials.resize(0);
                medals.resize(0);
                if (lines.size() == i) break;
                read = 's';
                numline = 1;
                i++;
                line = lines[i];
            } else if (line == "//") { //novo trial
                read = 't';
                trial =Trial();
                numline = 1;
                i++;
                line = lines[i];
                trialPlayers.resize(0);
                trialCountries.resize(0);
            }
        }
        //ler sport
        if (read == 's') {
            switch (numline) {
                case 1:
                    if (checkStringInput(line) != 0)
                        throw FileStructureError(competitionsFilename);
                    name = line;
                    break;
                case 2:
                    if (checkPositiveIntInput(line) != 0) //check int input
                        throw FileStructureError(competitionsFilename);
                    if (stoi(line) == 1) {
                        //individual sport
                        individualSport = new IndividualSport();
                        individualSport->setName(name);
                        isTeamSport = false;
                    } else if (stoi(line) > 1) {
                        //team Sport
                        teamSport = new TeamSport();
                        teamSport->setName(name);
                        teamSport->setNumberofElements(stoi(line));
                        isTeamSport = true;
                    } else // se for 0
                        throw FileStructureError(competitionsFilename);
                    numline = 0;
                    break;
                default:
                    throw FileStructureError(competitionsFilename);
            }
        }
        //ler competição
        if (read == 'c') {
            switch (numline) {
                case 1:
                    competition.setName(line);
                    break;
                case 2:
                    if (checkDateInput(line, d) != 0)
                        throw FileStructureError(competitionsFilename);
                    if (d.isOlimpianDate())
                        competition.setBegin(d);
                    else
                        throw FileStructureError(competitionsFilename);
                    break;
                case 3:
                    if (checkDateInput(line, d) != 0)
                        throw FileStructureError(competitionsFilename);
                    if (d.isOlimpianDate()) {
                        competition.setEnd(d);
                    } else {
                        throw FileStructureError(competitionsFilename);
                    }
                    break;
                case 4:
                    //ler competições - confirmar estrutura
                    participantsStream.str(line);
                    if(line != "-1"){
                        while (getline(participantsStream, name, ',')) {
                            if (name.find('-') != string::npos) {
                                pCountry = name.substr(0, name.find('-'));
                                pCountry = regex_replace(pCountry, regex("^ +| +$|( ) +"), "$1");
                                participant = name.substr(name.find('-') + 1, name.size());
                                participant = regex_replace(participant, regex("^ +| +$|( ) +"), "$1");
                                medal.setWinner(participant);
                                medal.setCountry(pCountry);
                                if (medalCount == 0)
                                    medal.setType('g');
                                else if (medalCount == 1)
                                    medal.setType('s');
                                else
                                    medal.setType('b');
                                medals.push_back(medal);
                                medalCount++;
                            }
                        }
                    }else medals.resize(0);

                    if(i < lines.size()-1 && !isTeamSport){//se não for a última linha, for um desporto individual e
                        i++;
                        if(!lines[i].empty() && lines[i] != "////////" && lines[i] != "//" ){ //se não for o fim do ficheiro, o inicio de um jogo,
                            line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");//de uma nova modalidade ou de uma nova competição então é um resultado
                            if (checkFloatInput(line) != 0)
                                throw FileStructureError(competitionsFilename);
                            competition.setResult(stof(line));
                        }else{
                            i--;
                            competition.setResult();
                        }
                    }
                    else{
                        competition.setResult();//valor default -2 -> não utiliza o sistema de resultados
                    }
                    participantsStream.clear();
                    numline = 0;
                    medalCount = 0;
                    break;
                default:
                    throw FileStructureError(competitionsFilename);
            }
        }
        //ler jogo
        if (read == 't') {
            switch (numline) {
                case 1:
                    trial.setName(line);
                    break;
                case 2:
                    if (checkPositiveIntInput(line) != 0)
                        throw FileStructureError(competitionsFilename);
                    if (stoi(line) != 0)
                        trial.setNumberOfElements(stoi(line));
                    break;
                case 3:
                    if (checkDateInput(line, d) != 0)
                        throw FileStructureError(competitionsFilename);
                    trial.setDate(d);
                    break;
                case 4:
                    participantsStream.str(line);
                    while (getline(participantsStream, name, ',')) {
                        if (name.find('-') != string::npos) {
                            trialCountries.push_back(regex_replace(name.substr(0, name.find('-')), regex("^ +| +$|( ) +"), "$1"));
                            trialPlayers.push_back(regex_replace(name.substr(name.find('-') + 1, name.size()), regex("^ +| +$|( ) +"), "$1"));
                        }
                    }
                    trial.setPlayers(trialPlayers);
                    trial.setCountries(trialCountries);
                    participantsStream.clear();
                    break;
                case 5:
                    if(line != "-1")
                        trial.setWinner(line);
                    else trial.setWinner("");

                    if(i < lines.size()-1 && !isTeamSport){//se não for a última linha, for um desporto individual e
                        i++;
                        if(!lines[i].empty() && lines[i] != "////////" && lines[i] != "//" ){ //se não for o fim do ficheiro, o inicio de um jogo,
                            line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");//de uma nova modalidade ou de uma nova competição então é um resultado
                            if (checkFloatInput(line) != 0)
                                throw FileStructureError(competitionsFilename);
                            trial.setResult(stof(line));
                        }
                        else{
                            i--;
                        }
                    }
                    else{
                        trial.setResult();//valor default -2 -> não utiliza o sistema de resultados
                    }

                    numline = 0;
                    trials.push_back(trial);

                    break;
                default:
                    throw FileStructureError(competitionsFilename);
            }
        }
    }
}

void Delegation::writeCompetitionsFile(){

    ofstream myfile (competitionsFilename + ".txt");
    if (myfile.is_open()) {
        for (int i = 0; i < sports.size(); ++i) {
            myfile << sports.at(i)->getName() << endl;
            if (sports.at(i)->isTeamSport()) {
                TeamSport *sport = dynamic_cast<TeamSport *> (sports.at(i));
                myfile << sport->getNumberofElements() << "\n\n";
            } else {
                IndividualSport *sport = dynamic_cast<IndividualSport *> (sports.at(i));
                myfile << 1 << "\n\n";
            }
            for (int j = 0; j < sports.at(i)->getCompetitions().size(); ++j) {
                myfile << sports.at(i)->getCompetitions().at(j).getName() << endl;
                myfile << sports.at(i)->getCompetitions().at(j).getBegin() << endl;
                myfile << sports.at(i)->getCompetitions().at(j).getEnd() << endl;

                if(sports.at(i)->getCompetitions().at(j).getMedals().empty()){
                    myfile << "-1";
                    if (i != sports.size() -1)  // se não for o ultimo deporto adicona nova linha depois das medalhas da competicao
                        myfile << "\n";
                    if (i == sports.size() -1)  // se for o ultimo desporto
                    {
                        if(j != sports.at(i)->getCompetitions().size() - 1)
                            myfile << "\n";
                    }
                }
                for (int k = 0; k < sports.at(i)->getCompetitions().at(j).getMedals().size(); ++k) {
                    myfile << sports.at(i)->getCompetitions().at(j).getMedals().at(k).getCountry() << "-"
                           << sports.at(i)->getCompetitions().at(j).getMedals().at(k).getWinner();
                    if (k != sports.at(i)->getCompetitions().at(j).getMedals().size() - 1)
                        myfile << ",";
                    else{
                        if (i != sports.size() -1)  // se não for o ultimo deporto adicona nova linha depois das medalhas da competicao
                            myfile << "\n";
                        if (i == sports.size() -1)  // se for o ultimo desporto
                        {
                            if(j != sports.at(i)->getCompetitions().size() - 1)
                                myfile << "\n";
                        }
                    }

                }
                if(sports.at(i)->getCompetitions().at(j).getResult() != -2.0)
                    myfile << sports.at(i)->getCompetitions().at(j).getResult()<<endl;
                if(sports.at(i)->getCompetitions().at(j).getResult() == -2.0 && sports.at(i)->getCompetitions().at(j).getTrials().size() != 0 && !sports.at(i)->isTeamSport())
                    myfile << endl;
                if (sports.at(i)->getCompetitions().at(j).getTrials().size() != 0) {
                    for (int k = 0; k < sports.at(i)->getCompetitions().at(j).getTrials().size(); ++k) {
                        myfile << "//" << endl;
                        myfile << sports.at(i)->getCompetitions().at(j).getTrials().at(k).getName() << endl;
                        myfile << sports.at(i)->getCompetitions().at(j).getTrials().at(k).getParticipants().size()<< endl;
                        myfile << sports.at(i)->getCompetitions().at(j).getTrials().at(k).getDate() << endl;
                        for (int l = 0; l < sports.at(i)->getCompetitions().at(j).getTrials().at(k).getParticipants().size(); ++l) {
                            myfile << sports.at(i)->getCompetitions().at(j).getTrials().at(k).getCountries().at(l);
                            myfile << "-";
                            myfile << sports.at(i)->getCompetitions().at(j).getTrials().at(k).getParticipants().at(l);
                            if (l !=sports.at(i)->getCompetitions().at(j).getTrials().at(k).getParticipants().size() - 1)
                                myfile << ",";
                            else{
                                if (i != sports.size() -1)  // se não for o ultimo deporto adicona nova linha
                                    myfile << "\n";
                                else // se for o ultimo desporto
                                {
                                    if(j != sports.at(i)->getCompetitions().size() - 1)//se nao for a ultima competiçao
                                        myfile << "\n";
                                    else{
                                        if(k !=sports.at(i)->getCompetitions().at(j).getTrials().size() - 1 || sports.at(i)->getCompetitions().at(j).getTrials().at(k).getResult() != -2.0)
                                            myfile << "\n";
                                    }
                                }
                            }

                        }
                        if(sports.at(i)->getCompetitions().at(j).getTrials().at(k).getParticipants().empty()) myfile << endl;
                        if(sports.at(i)->getCompetitions().at(j).getTrials().at(k).getWinner() == "")
                            myfile << "-1";
                        else myfile << sports.at(i)->getCompetitions().at(j).getTrials().at(k).getWinner();
                        if(sports.at(i)->getCompetitions().at(j).getTrials().at(k).getResult() != -2.0){//tem result
                            myfile << endl  << sports.at(i)->getCompetitions().at(j).getTrials().at(k).getResult();
                        }
                        if (i != sports.size() -1)  // se não for o ultimo deporto adicona nova linha
                            myfile << "\n";
                        else // se for o ultimo desporto
                        {
                            if(j != sports.at(i)->getCompetitions().size() - 1)//se nao for a ultima competiçao
                                myfile << "\n";
                            else{
                                if(k != sports.at(i)->getCompetitions().at(j).getTrials().size() - 1)//se nao for o ultimo trial
                                    myfile << "\n";
                            }
                        }
                    }
                } else {
                    if (j != sports.at(i)->getCompetitions().size() - 1)
                        myfile << "\n";
                }
            }
            if (i != sports.size()-1)
                myfile << "////////" << endl;
        }
    }
}

void Delegation::readTeamsFile(const vector<string> &lines) {
    int numline = 0;
    string line;
    Date d;
    bool readNewTeam = false;
    Team *t = nullptr;
    //Variables to read Athletes:
    istringstream membersStream;
    string memberStr;
    vector<Athlete*> members;
    vector<Athlete*>::iterator it;
    Athlete* a;
    string sport;

    for (size_t i = 0; i < (lines.size()+1); i++) {
        numline++;
        if (i==lines.size()) {
            teams.push_back(new Team(*t));
            break;
        }
        line = lines[i];
        if (line.empty()) { // Se alinha está vazia voltamos a colocar o numLines a 0 para ler a próxima equipa
            teams.push_back(new Team(*t));
            numline = 1;
            i++;
            line = lines[i];
            readNewTeam = true;
            membersStream.clear();
            members.resize(0);
        } else if (line == "--------") {
            teams.push_back(new Team(*t));
            numline = 1;
            i++;
            line = lines[i];
            membersStream.clear();
            members.resize(0);
            readNewTeam = false;
        }

        if (numline == 1) {
            t = new Team();
        }

        //ler equipa do desporto atual ou equipas de desporto novo
        if (!readNewTeam) { // ler desporto novo
            switch (numline) {
                case 1:
                    if (checkStringInput(line) != 0)
                        throw FileStructureError(teamsFilename);
                    sport = line;
                    readNewTeam = true;
                    numline=0;
                    break;
            }
        } else {
            // ler equipa nova
            switch (numline) {
                case 1:
                    if (checkAlphaNumericInput(line) != 0) //check team name input - can have numbers
                        throw FileStructureError(teamsFilename);
                    t->setName(line);
                    t->setSport(sport);
                    break;
                case 2:
                    //ler competições - confirmar estrutura
                    membersStream.str(line);
                    while (getline(membersStream, memberStr, ',')) {
                        memberStr = regex_replace(memberStr, regex("^ +| +$|( ) +"), "$1");
                        if (checkStringInput(memberStr) != 0)
                            throw FileStructureError(teamsFilename);
                        for (it = athletes.begin(); it != athletes.end(); it++) {
                            if ((*it)->getName() == memberStr){
                                a = *it;
                                members.push_back(a);
                            }
                        }
                    }
                    t->setAthletes(members);
                    break;
            }
        }
    }
}

void Delegation::writeTeamsFile() {
    ofstream myfile (teamsFilename + ".txt");
    int nTeamSports = 0,tmp=0;
    if (myfile.is_open())
    {
        for(unsigned int i = 0; i < sports.size(); ++i){
            if(sports[i]->isTeamSport())
                nTeamSports++;
        }
        for (unsigned int i = 0; i < sports.size(); ++i) {
            TeamSport* teamsp = dynamic_cast<TeamSport*>(sports.at(i));
            if (teamsp != NULL){
                myfile << sports.at(i)->getName() << endl;
                for (int j = 0; j < teamsp->getTeams().size(); ++j) {
                    myfile << teamsp->getTeams().at(j)->getName() << endl;
                    for (int k = 0; k < teamsp->getTeams().at(j)->getAthletes().size(); ++k) {
                        myfile << teamsp->getTeams().at(j)->getAthletes().at(k)->getName();
                        if (k != teamsp->getTeams().at(j)->getAthletes().size() -1)
                            myfile << ", ";
                    }
                    if (j != teamsp->getTeams().size()-1)
                        myfile << "\n\n";
                }
                if (tmp != nTeamSports-1)
                    myfile << "\n--------\n";
                tmp++;
            }
        }
        myfile.close();
    }
    else cerr << "Unable to open file";
}

void Delegation::writeDelegationFile() {
    ofstream myfile (delegationFilename + ".txt");
    if (myfile.is_open())
    {
        myfile << country << endl << dailyCostAthlete << endl << dailyCostStaff << endl << totalCost << endl << peopleFilename << endl << teamsFilename <<endl << competitionsFilename<< endl<<recordsFilename;
        myfile.close();
    }
    else cerr << "Unable to open file";
}

void Delegation::readRecordsFile(const vector<string> &lines) {
    int numline = 0;
    string line;
    Date d;
    char read='s'; // auxiliar para saber se vamos ler um record de uma competition ou de um trial (c ou t) ou nome de um sport (s)
    Record record;
    for (size_t i = 0; i < lines.size()+1; i++) {
        numline++;
        if (i != lines.size())
            line = lines[i];
        else{
            records.insert(record);
            break;
        }
        if (numline == 1 && (line.empty()||line == "////////"||line == "//")) {// se for a primeira linha vamos ver se é uma nova modalidade, competição ou jogo
            if(line == "////////"){
                records.insert(record);
                read ='s';
                i++;
                line = lines[i];
            }
            else if(line =="//"){
                records.insert(record);
                read = 't';
                i++;
                line = lines[i];
            }
            else if( line.empty()){
                records.insert(record);
                read = 'c';
                i++;
                line = lines[i];
            }
        }

        if(read == 's'){
            line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
            if (checkStringInput(line) != 0)
                throw FileStructureError(recordsFilename);
            record.setSport(line);
            numline = 0;
            if(lines[i+1].find("-") != string::npos){//we are reading a trial
                read = 't';
            }
            else{
                read = 'c';
            }
        }
        else if (read == 'c'){
            switch(numline){
                case 1:
                    line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
                    record.setCompetition(line);
                    record.setTrial();
                    break;
                case 2:
                    if (line != "+" && line != "-")
                        throw FileStructureError(recordsFilename);
                    record.setComparisonCriteria(line[0]);
                    break;
                case 3:
                    line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
                    if (checkDateInput(line, d) != 0){
                        if(line == "-1") d = Date();
                        else throw FileStructureError(recordsFilename);
                    }
                    record.setDate(d);
                    break;
                case 4:
                    line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
                    if(line == "-1") line = "";
                    record.setPlace(line);
                    break;
                case 5:
                    if (line.find('-') != string::npos) {
                        if(line == "-1"){
                            line = "";
                            record.setCountry(line);
                            record.setRecordist(line);
                        }
                        else{
                            record.setCountry(regex_replace(line.substr(0, line.find('-')), regex("^ +| +$|( ) +"), "$1"));
                            record.setRecordist(regex_replace(line.substr(line.find('-') + 1, line.size()), regex("^ +| +$|( ) +"),"$1"));
                        }
                    }
                    else throw FileStructureError(recordsFilename);
                    break;
                case 6:
                    line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
                    if (checkFloatInput(line) != 0)
                        throw FileStructureError(recordsFilename);
                    record.setRecord(stof(line));
                    numline=0;
                    break;
                default:
                    throw FileStructureError(recordsFilename);
            }
        }
        else{
            switch(numline){
                case 1:
                    if (line.find('-') != string::npos) {
                        record.setCompetition(regex_replace(line.substr(0, line.find('-')), regex("^ +| +$|( ) +"), "$1"));
                        record.setTrial(regex_replace(line.substr(line.find('-') + 1, line.size()), regex("^ +| +$|( ) +"),"$1"));
                    }
                    else throw FileStructureError(recordsFilename);
                    break;
                case 2:
                    line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
                    if (line != "+" && line != "-")
                        throw FileStructureError(recordsFilename);
                    record.setComparisonCriteria(line[0]);
                    break;
                case 3:
                    line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
                    if (checkDateInput(line, d) != 0){
                        if(line == "-1") d = Date();
                        else throw FileStructureError(recordsFilename);
                    }
                    record.setDate(d);
                    break;
                case 4:
                    line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
                    if(line == "-1") line = "";
                    record.setPlace(line);
                    break;
                case 5:
                    if (line.find('-') != string::npos) {
                        if(line == "-1"){
                            line = "";
                            record.setCountry(line);
                            record.setRecordist(line);
                        }
                        else{
                            record.setCountry(regex_replace(line.substr(0, line.find('-')), regex("^ +| +$|( ) +"), "$1"));
                            record.setRecordist(regex_replace(line.substr(line.find('-') + 1, line.size()), regex("^ +| +$|( ) +"),"$1"));
                        }
                    }
                    else throw FileStructureError(recordsFilename);
                    break;
                case 6:
                    line = regex_replace(lines[i], regex("^ +| +$|( ) +"), "$1");
                    if (checkFloatInput(line) != 0)
                        throw FileStructureError(recordsFilename);
                    record.setRecord(stof(line));
                    numline=0;
                    break;
                default:
                    throw FileStructureError(recordsFilename);
            }
        }
    }
}

void Delegation::writeRecordsFile() {
    ofstream myfile (recordsFilename + ".txt");
    if (myfile.is_open())
    {
        BSTItrIn<Record> it(records);
        BSTItrIn<Record> it2(records);
        string sp="";
        string c="";
        int compCount=1;
        int trialCount = 0;
        int nRecords=0;

        while(!it2.isAtEnd()){
            nRecords++;
            it2.advance();
        }

        while(!it.isAtEnd()){
            nRecords--;
            if(it.retrieve().getSport()!= sp) {
                if (sp != ""){
                    myfile << "////////"<<endl;
                    compCount = 1;
                    trialCount = 0;
                }
                sp = it.retrieve().getSport();
                myfile << sp << endl;
            }
            else{
                if(c != it.retrieve().getCompetition()){
                    c=it.retrieve().getCompetition();
                    compCount++;
                    trialCount++;
                }
                else{
                    trialCount++;
                }
            }

            if(it.retrieve().getTrial() == ""){//competition
                if(compCount != 1) myfile << endl; //separa competições por linha vazia
                myfile << it.retrieve().getCompetition()<<endl;
                myfile << it.retrieve().getComparisonCriteria()<<endl;
                if(it.retrieve().getDate() == Date()) myfile << "-1\n";
                else myfile << it.retrieve().getDate()<<endl;
                if(it.retrieve().getPlace().empty()) myfile << "-1\n";
                else myfile << it.retrieve().getPlace()<<endl;
                if(it.retrieve().getCountry().empty() && it.retrieve().getRecordist().empty()) myfile << "-1\n";
                else myfile << it.retrieve().getCountry() << "-" << it.retrieve().getRecordist()<<endl;
                myfile << it.retrieve().getRecord();
                if(nRecords != 0) myfile << endl;
            }
            else{
                if(trialCount != 0) myfile << "//"<<endl; //separa os trials
                myfile << it.retrieve().getCompetition()<< "-"<< it.retrieve().getTrial()<<endl;
                myfile << it.retrieve().getComparisonCriteria()<<endl;
                if(it.retrieve().getDate() == Date()) myfile << "-1\n";
                else myfile << it.retrieve().getDate()<<endl;
                if(it.retrieve().getPlace().empty()) myfile << "-1\n";
                else myfile << it.retrieve().getPlace()<<endl;
                if(it.retrieve().getCountry().empty() && it.retrieve().getRecordist().empty()) myfile << "-1\n";
                else myfile << it.retrieve().getCountry() << "-" << it.retrieve().getRecordist()<<endl;
                myfile << it.retrieve().getRecord();
                if(nRecords != 0) myfile << endl;
            }

            it.advance();
        }
    }
    else cerr << "Unable to open file";
}

//Acessors and mutators
Record Delegation::getRecord(string competition, string sport) {
    Record itemNotFound;
    Record toFindRecord(sport, competition);
    BSTItrIn<Record> it(records);
    while (!it.isAtEnd())
    {
        if( it.retrieve() == toFindRecord) {
            Record pti(it.retrieve());
            return pti;
        }
        it.advance();
    }
    return itemNotFound;
}

void Delegation::addRecord(const Record & record) {
    Record itemNotFound;
    Record foundOrNot = records.find(record);
    if(foundOrNot == itemNotFound) {
        records.insert(record);
    }
    else {
        records.remove(foundOrNot);
        records.insert(record);
    }
}

const string &Delegation::getCountry() const {
    return country;
}

void Delegation::setCountry(const string &count) {
    this->country = count;
}

float Delegation::getDailyCostAthlete() const {
    return dailyCostAthlete;
}

void Delegation::setDailyCostAthlete(float dailyCost) {
    this->dailyCostAthlete = dailyCost;
}

float Delegation::getDailyCostStaff() const {
    return dailyCostStaff;
}

void Delegation::setDailyCostStaff(float dailyCost) {
    this->dailyCostStaff = dailyCost;
}

float Delegation::getTotalCost() const {
    return totalCost;
}

void Delegation::setTotalCost(float totalcost) {
    this->totalCost = totalcost;
}

const vector<Sport *> &Delegation::getSports() const {
    return sports;
}

void Delegation::calculateTotalCost() {
    float result = 0;

    for (auto &i : people) {
        if (i->isAthlete()) {
            result += (float) daysBetween(i->getArrival(), i->getDeparture()) * dailyCostAthlete;
        } else {
            result += (float) daysBetween(i->getArrival(), i->getDeparture()) * dailyCostStaff;
        }
    }

    this->totalCost = result;
}

string Delegation::info() const {
    ostringstream os;
    os << left << setw(17) << "Country" << setw(4) << " " << country << setw(3) << endl;
    os << left << setw(17) << "Staff's Daily Cost" << setw(4) << " " << dailyCostStaff << setw(3) << endl;
    os << left << setw(17) << "Athlete's Daily Cost" << setw(4) << " " << dailyCostAthlete << setw(3) << endl;
    os << left << setw(17) << "Total Cost" << setw(4) << " " << totalCost << setw(3) << endl;
    return os.str();
}

void Delegation::showMembers() {
    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(26) << " " <<"Delegation Members" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;


    if (!people.empty()) {
        vector<Person *>::const_iterator it;
        for (it = people.begin(); it != people.end(); it++) {
            (*it)->showInfoPerson();
            cout << endl;
        }
    } else
        throw NoMembers();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

int Delegation::findPerson(const string & name) const {
    for (int i = 0; i < people.size(); i++) {
        if (name == people.at(i)->getName()) return i;
    }
    return -1;
}

staffHtabcit Delegation::FindPersonHash(const string & name) const {
    std::unordered_set<Staff*, staffHash, staffHash>::const_iterator a;
    Staff* temp = new Staff(name);
    a = staff.find(temp);
    return a;
}

//Staff Functions
void Delegation::addStaffMember() {
    Staff* novo = new Staff();
    string tmp;
    Date tmp_date;
    pair<staffHtabit ,bool> testinsert;

    int test = 0;
    string input = "";

    cout << "Name: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp)) {
        cerr << "Invalid Name. Try again!" << endl;
        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    if (findPerson(tmp) != -1) {
        throw PersonAlreadyExists(tmp);
    }
    novo->setName(tmp);

    cout << "Date of Birth: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkDateInput(tmp, tmp_date)) {
        cout << "Invalid Date. Try again!" << endl;
        cout << "Date of Birth: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setBirth(tmp_date);

    cout << "Passport: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkAlphaNumericInput(tmp)) {
        cout << "Invalid Passport. Try again!" << endl;
        cout << "Passport: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setPassport(tmp);

    cout << "Date of Arrival: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkDateInput(tmp, tmp_date) || !(tmp_date.isOlimpianDate())) {
        cout << "Invalid Date. Try again!" << endl;
        cout << "Date of Arrival: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setArrival(tmp_date);

    cout << "Date of Departure: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    Date arrival = novo->getArrival();
    while (checkDateInput(tmp, tmp_date) || !(tmp_date.isOlimpianDate()) || !(dateIsBefore(arrival,tmp))) {
        cout << "Invalid Date. Try again!" << endl;
        cout << "Date of Departure: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setDeparture(tmp_date);

    cout << "Function: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp) == 1) {
        cout << "Invalid Function. Try again!" << endl;
        cout << "Function: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setFunction(tmp);

    cout << "Employed (1-Yes 0-No): ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (tmp != "0" && tmp != "1") {
        cout << "Invalid value. Try again!" << endl;
        cout << "Employed (1-Yes 0-No): ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setEmployed(to_bool(tmp));

    if(novo->getEmployed()) {
        cout << "Availability: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
        while (checkFloatInput(tmp) || stof(tmp) < 0) {
            cerr << "Invalid Availability. Try again!" << endl;
            cout << "Availability: ";
            getline(cin, tmp);
            if (cin.eof()) {
                cin.clear();
                return; //go back on ctrl+d
            }
            cin.clear();
        }
        novo->setAvailability(stof(tmp));
    } else {
        novo->setAvailability(-1);
    }

    people.push_back(novo);
    staff.insert(novo);
    staffService.push(novo);

    cout << endl << "Staff Member added with success!" << endl;
    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::removeStaffMember() {
    int test = 0;
    int index;
    string input = "", tmp;

    cout << "Name: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp)) {
        cout << "Invalid Name. Try again!" << endl;
        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    index = findPerson(tmp);
    if (index == -1 || people.at(index)->isAthlete()) {
        throw NonExistentStaff(tmp);
    } else {
        vector<Person *>::iterator it = people.begin() + index;
        delete *it;
        people.erase(it);

        staffHtabcit toerase = FindPersonHash(tmp);
        if (toerase != staff.end()) staff.erase(toerase);

        priority_queue<Staff*, vector<Staff*>, Compare> tmp_queue;
        while(!staffService.empty()){
            if(staffService.top()->getName() == tmp){
                staffService.pop();
            } else {
                tmp_queue.push(staffService.top());
                staffService.pop();
            }
        }
        staffService = tmp_queue;

        cout << endl << "Staff Member removed with success!" << endl;
        cout << endl << "0 - BACK" << endl;
        do {
            test = checkinputchoice(input, 0, 0);
            if (test != 0&& test != 2)
                cerr << "Invalid option! Press 0 to go back." << endl;
        } while (test != 0 && test != 2);
        return;
    }
}

void Delegation::changeStaffMember() {
    int test = 0;
    int index;
    string input = "", tmp;

    cout << "Name: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp)) {
        cout << "Invalid Name. Try again!" << endl;
        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }

    index = findPerson(tmp);
    staffHtabcit tochange = FindPersonHash(tmp);
    Staff* toc = *tochange;
    priority_queue<Staff*, vector<Staff*>, Compare> tmp_queue;

    if (index == -1 || people.at(index)->isAthlete()) {
        throw NonExistentStaff(tmp);
    } else {
        Date departure = people[index]->getDeparture();
        Date arrival = people[index]->getArrival();
        system("cls");
        cout << "_____________________________________________________" << endl << endl;
        cout << "\t\t   What do you want to change?" << endl;
        cout << "_____________________________________________________" << endl << endl;

        cout << "1 - Name" << endl;
        cout << "2 - Date of Birth" << endl;
        cout << "3 - Passport" << endl;
        cout << "4 - Date of Arrival" << endl;
        cout << "5 - Date of Departure" << endl;
        cout << "6 - Function" << endl;
        cout << "7 - Employment" << endl;
        cout << "8 - Availability" << endl;
        cout << "0 - BACK" << endl;

        do {
            test = checkinputchoice(input, 0, 8);
            if (test != 0 && test != 2)
                cerr << "Invalid option! Please try again." << endl;
        } while (test != 0 && test != 2);
        if (test == 2) { input = "0"; }

        Date tmp_date;

        switch (stoi(input)) {
            case 1:
                cout << "New name: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkStringInput(tmp) || findPerson(tmp) != -1) {
                    cout << "Invalid Name. Try again!" << endl;
                    cout << "New name: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }

                staff.erase(tochange);
                toc->setName(tmp);
                staff.insert(toc);

                //people.at(index)->setName(tmp);
                break;
            case 2:
                cout << "Date of Birth: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkDateInput(tmp, tmp_date)) {
                    cout << "Invalid Date. Try again!" << endl;
                    cout << "Date of Birth: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }

                staff.erase(tochange);
                toc->setBirth(tmp_date);
                staff.insert(toc);

                //people.at(index)->setBirth(tmp_date);
                break;
            case 3:
                cout << "Passport: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkAlphaNumericInput(tmp)) {
                    cout << "Invalid Passport. Try again!" << endl;
                    cout << "Passport: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }

                staff.erase(tochange);
                toc->setPassport(tmp);
                staff.insert(toc);

                //people.at(index)->setPassport(tmp);
                break;
            case 4:
                cout << "Date of Arrival: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkDateInput(tmp, tmp_date) || !(tmp_date.isOlimpianDate()) || !(dateIsAfter(departure,tmp))) {
                    cout << "Invalid Date. Try again!" << endl;
                    cout << "Date of Arrival: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }

                staff.erase(tochange);
                toc->setArrival(tmp_date);
                staff.insert(toc);

                //people.at(index)->setArrival(tmp_date);
                break;
            case 5:
                cout << "Date of Departure: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkDateInput(tmp, tmp_date) || !(tmp_date.isOlimpianDate())|| !(dateIsBefore(arrival,tmp))) {
                    cout << "Invalid Date. Try again!" << endl;
                    cout << "Date of Departure: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }

                staff.erase(tochange);
                toc->setDeparture(tmp_date);
                staff.insert(toc);

                //people.at(index)->setDeparture(tmp_date);
                break;
            case 6:
                cout << "Function: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkStringInput(tmp)) {
                    cout << "Invalid Function. Try again!" << endl;
                    cout << "Function: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }

                staff.erase(tochange);
                toc->setFunction(tmp);
                staff.insert(toc);

                /*if (!people.at(index)->isAthlete()) {
                    Staff *s = dynamic_cast<Staff *> (people.at(index));
                    if (s == NULL) {
                        cout << "Couldn't change function!" << endl;
                    } else
                        s->setFunction(tmp);
                }*/

                break;
            case 7:
                cout << "Employed (1-Yes 0-No): ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (tmp != "0" && tmp != "1") {
                    cout << "Invalid value. Try again!" << endl;
                    cout << "Employed (1-Yes 0-No): ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }

                staff.erase(tochange);
                toc->setEmployed(to_bool(tmp));
                if (to_bool(tmp)) {
                    toc->setAvailability(0);
                } else {
                    toc->setAvailability(-1);
                }
                staff.insert(toc);

                while(!staffService.empty()){
                    if(staffService.top()->getEmployed()) tmp_queue.push(staffService.top());
                    staffService.pop();
                }
                staffService = tmp_queue;
                while(!tmp_queue.empty()){
                    tmp_queue.pop();
                }
                break;
            case 8:
                if(toc->getEmployed()) {
                    cout << "Availability: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                    while (checkFloatInput(tmp) || stof(tmp) < 0) {
                        cerr << "Invalid Availability. Try again!" << endl;
                        cout << "Availability: ";
                        getline(cin, tmp);
                        if (cin.eof()) {
                            cin.clear();
                            return; //go back on ctrl+d
                        }
                        cin.clear();
                    }
                    staff.erase(tochange);
                    toc->setAvailability(stof(tmp));
                    staff.insert(toc);

                    while(!staffService.empty()){
                        tmp_queue.push(staffService.top());
                        staffService.pop();
                    }
                    staffService = tmp_queue;
                    while(!tmp_queue.empty()){
                        tmp_queue.pop();
                    }
                } else {
                    cout << "Staff Member is not hired at the moment!" << endl;
                }
            case 0:
                break;
            default:
                break;
        }

        cout << endl << "Staff Member changed with success!" << endl;
        cout << endl << "0 - BACK" << endl;
        do {
            test = checkinputchoice(input, 0, 0);
            if (test != 0&& test != 2)
                cerr << "Invalid option! Press 0 to go back." << endl;
        } while (test != 0 && test != 2);
    }
}

void Delegation::showStaffMember() const {
    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(19) << " "<<"Information about a Staff Member" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    if (!staff.empty()) {
        // to use with vector people
        /*int test = 0;
        int index;*/
        string input = "", tmp;

        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
        while (checkStringInput(tmp)) {
            cerr << "Invalid Name. Try again!" << endl;
            cout << "Name: ";
            getline(cin, tmp);
            if (cin.eof()) {
                cin.clear();
                return; //go back on ctrl+d
            }
            cin.clear();
        }

        staffHtabcit toview = FindPersonHash(tmp);
        if (toview == staff.end())
            throw NonExistentStaff(tmp);
        else{
            cout << endl;
            (*toview)->showInfoPerson();
        }

        // to use with vector people
        /*index = findPerson(tmp);
        if (index == -1 || people.at(index)->isAthlete())
            throw NonExistentStaff(tmp);
        else {
            cout << endl;
            (*(people.begin() + index))->showInfoPerson();
        }*/
    } else
        throw NoMembers();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showStaffMembers() {
    int test = 0;
    string input = "";

    system("cls");
    if(!staff.empty()){
        cout << "----------------------------------------------------------------------" << endl;
        cout << setw(19) << " " <<"Information about Staff Members" << endl;
        cout << "----------------------------------------------------------------------" << endl << endl;

        cout << "1 - Employed" << endl;
        cout << "2 - Not Employed" << endl;
        cout << "0 - BACK" << endl;

        do {
            test = checkinputchoice(input, 0, 2);
            if (test != 0 && test != 2)
                cerr << "Invalid option! Please try again." << endl;
        } while (test != 0 && test != 2);
        if (test == 2) { input = "0"; }

        if (input != "0"){
            staffHtabit it = staff.begin();
            while (it != staff.end()) {
                if (input == "1" && (*it)->getEmployed()){
                    (*it)->showInfoPerson();
                    cout << endl;
                }
                if (input == "2" && !(*it)->getEmployed()){
                    (*it)->showInfoPerson();
                    cout << endl;
                }
                it++;
            }
        }

    }
    else
        throw NoMembers();

    /*if (!people.empty()) {
        sort(people.begin(), people.end(), sortMembersAlphabetically<Person>);
        vector<Person *>::const_iterator it;
        for (it = people.begin(); it != people.end(); it++) {
            if (!(*it)->isAthlete()) {
                (*it)->showInfoPerson();
                cout << endl;
            }
        }
    } else
        throw NoMembers();*/

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0 && test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::requireStaffService() {
    int test = 0;
    string input = "";
    string function, st;
    float service_time;
    priority_queue<Staff*, vector<Staff*>, Compare> tmp_queue;

    cout << "Function (if there are no staff members with such function, the first one ready will be required): ";
    getline(cin, function);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(function)) {
        cout << "Invalid Function. Try again!" << endl;
        cout << "Function (if there are no staff members with such function, the first one ready will be required): ";
        getline(cin, function);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }

    cout << "Time of service (hours): ";
    getline(cin, st);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkFloatInput(st) || stof(st) < 0) {
        cerr << "Invalid Time of service (hours). Try again!" << endl;
        cout << "Time of service (hours): ";
        getline(cin, st);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    service_time = stof(st);

    while(!staffService.empty()){
        if(staffService.top()->getFunction() == function){
            break;
        }
        tmp_queue.push(staffService.top());
        staffService.pop();
    }

    if(staffService.empty()){
        cout << tmp_queue.top()->getName() << " has been required for your service!" << endl;
        tmp_queue.top()->setAvailability(service_time + tmp_queue.top()->getAvailability());
    } else {
        cout << staffService.top()->getName() << " has been required for your service!" << endl;
        staffService.top()->setAvailability(service_time + staffService.top()->getAvailability());
        tmp_queue.push(staffService.top());
        staffService.pop();
    }

    while(!tmp_queue.empty()){
        staffService.push(tmp_queue.top());
        tmp_queue.pop();
    }

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::endService(){
    int test = 0;
    int index;
    string input = "", tmp, st;

    cout << "Name: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp)) {
        cout << "Invalid Name. Try again!" << endl;
        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    index = findPerson(tmp);
    if (index == -1 || people.at(index)->isAthlete()) {
        throw NonExistentStaff(tmp);
    } else {
        priority_queue<Staff*, vector<Staff*>, Compare> tmp_queue;
        while(!staffService.empty()){
            if(staffService.top()->getName() == tmp){
                cout << "Time of service (hours): ";
                getline(cin, st);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkFloatInput(st) || stof(st) < 0 || stof(st) > staffService.top()->getAvailability()) {
                    cerr << "Invalid Time of service (hours). Try again!" << endl;
                    cout << "Time of service (hours): ";
                    getline(cin, st);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                staffService.top()->setAvailability(staffService.top()->getAvailability() - stof(st));
            }
            tmp_queue.push(staffService.top());
            staffService.pop();
        }
        staffService = tmp_queue;

        cout << endl << "Service ended with success!" << endl;
        cout << endl << "0 - BACK" << endl;
        do {
            test = checkinputchoice(input, 0, 0);
            if (test != 0&& test != 2)
                cerr << "Invalid option! Press 0 to go back." << endl;
        } while (test != 0 && test != 2);
        return;
    }
}

//Athletes Functions

void Delegation::addAthlete() {
    Athlete* novo = new Athlete();
    string tmp;
    Date tmp_date;
    vector<Competition> competitions;
    vector<string> competition_names;
    int index,indexTeam;

    int test = 0;
    string input = "";

    cout << "Name: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp)) {
        cerr << "Invalid Name. Try again!" << endl;
        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    if (findPerson(tmp) != -1) {
        throw PersonAlreadyExists(tmp);
    }
    novo->setName(tmp);

    cout << "Date of Birth: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkDateInput(tmp, tmp_date)) {
        cout << "Invalid Date. Try again!" << endl;
        cout << "Date of Birth: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setBirth(tmp_date);

    cout << "Passport: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkAlphaNumericInput(tmp)) {
        cout << "Invalid Passport. Try again!" << endl;
        cout << "Passport: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setPassport(tmp);

    cout << "Date of Arrival: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkDateInput(tmp, tmp_date) || !(tmp_date.isOlimpianDate())) {
        cout << "Invalid Date. Try again!" << endl;
        cout << "Date of Arrival: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setArrival(tmp_date);

    cout << "Date of Departure: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    Date arrival = novo->getArrival();
    while (checkDateInput(tmp, tmp_date) || !(tmp_date.isOlimpianDate()) || !(dateIsBefore(arrival,tmp))) {
        cout << "Invalid Date. Try again!" << endl;
        cout << "Date of Departure: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setDeparture(tmp_date);

    cout << "Sport: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp) == 1) {
        cout << "Invalid Sport. Try again!" << endl;
        cout << "Sport: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    index = findSport(tmp);
    if (index == -1){
        throw NonExistentSport(tmp);
    } else {
        novo->setSport(tmp);
    }

    cout << "Weight: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkFloatInput(tmp)) {
        cerr << "Invalid Weight. Try again!" << endl;
        cout << "Weight: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setWeight(stof(tmp));

    cout << "Height: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkPositiveIntInput(tmp)) {
        cerr << "Invalid Height. Try again!" << endl;
        cout << "Height: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    novo->setHeight(stoi(tmp));

    if(!(sports.at(index)->isTeamSport())){
        //se for competição individual
        //adiciona todas as competições ao atleta
        competitions = sports.at(index)->getCompetitions();
        for (int i = 0; i < competitions.size(); i++) {
            competition_names.push_back(competitions.at(i).getName());
        }
        novo->setCompetitions(competition_names);

        //adiciona o atleta às competições
        for(auto &sportComp: competitions){
            vector<string> participants = sportComp.getParticipants();
            vector<string> indComps = novo->getCompetitions();
            for(auto &indComp: indComps){
                if(indComp == sportComp.getName())
                    participants.push_back(novo->getName());
            }
            noRepeatVector(participants);
            sportComp.setParticipants(participants);
        }
        sports.at(index)->setCompetitions(competitions);
    } else {
        TeamSport *ts = dynamic_cast<TeamSport *> (sports.at(index));
        vector<Team *> tmp_teams = ts->getTeams();

        if(!tmp_teams.empty()){
            cout << "Possible Teams:";
            for (int i = 0; i < tmp_teams.size(); i++) {
                cout << tmp_teams.at(i)->getName();
                if(i!=tmp_teams.size()-1)
                    cout << ", ";
            }
            cout << endl;
        }

        cout << "Team: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
        while (checkAlphaNumericInput(tmp) == 1) {
            cout << "Invalid Team. Try again!" << endl;
            cout << "Team: ";
            getline(cin, tmp);
            if (cin.eof()) {
                cin.clear();
                return; //go back on ctrl+d
            }
            cin.clear();
        }
        indexTeam = findTeam(tmp);
        if (indexTeam == -1) {
            throw NonExistentTeam(tmp);
        } else {
            if((teams.at(indexTeam)->getAthletes()).size() == ts->getNumberofElements()){
                throw FullTeam(tmp);
            } else {
                competition_names = teams.at(indexTeam)->getCompetitions();
                novo->setCompetitions(competition_names);
                teams.at(index)->addAthlete(novo);
            }
        }
    }

    people.push_back(novo);
    athletes.push_back(novo);

    cout << endl << "Athlete added with success!" << endl;
    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::removeAthlete(){
    int index;
    string tmp,nmAt;
    int test = 0;
    string input = "";

    cout << "Name: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp)) {
        cout << "Invalid Name. Try again!" << endl;
        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    index = findPerson(tmp);
    if (index == -1 || !(people.at(index)->isAthlete())) {
        throw NonExistentAthlete(tmp);
    } else {
        string s = athletes.at(index)->getSport();
        int index_s = findSport(s);
        if(sports.at(index_s)->isTeamSport()){
            TeamSport *ts = dynamic_cast<TeamSport *> (sports.at(index_s));
            vector<Team *> tmp_teams = ts->getTeams();
            string t;
            for(int i = 0; i < tmp_teams.size(); i++){
                vector<Athlete*> tmp_athletes = tmp_teams[i]->getAthletes();
                for(int j = 0; j < tmp_athletes.size(); j++){
                    if (tmp_athletes[j]->getName() == tmp) t = tmp_teams[i]->getName();
                }
            }
            int index_t = findTeam(t);
            teams[index_t]->removeAthlete(tmp);
        }
        vector<Person *>::iterator it = people.begin() + index;
        vector<Athlete*>::iterator it_a = athletes.begin() + index;
        nmAt = (*it_a)->getName();

        if(!(sports.at(index_s)->isTeamSport())){
            //elimina o participante das competições
            vector<Competition> comps = sports.at(index_s)->getCompetitions();
            vector<Competition> competitionsToSet;
            for(size_t i=0; i< comps.size(); i++){
                vector<string>participants = comps[i].getParticipants();
                vector<string>::iterator part_it = find(participants.begin(), participants.end(),nmAt);
                if(part_it != participants.end()){
                    participants.erase(part_it);
                }
                comps[i].setParticipants(participants);
                competitionsToSet.push_back(Competition(comps[i]));
            }
            sports.at(index_s)->setCompetitions(competitionsToSet);
        }
        athletes.erase(it_a);
        delete *it;
        people.erase(it);

        cout << endl << "Athlete removed with success!" << endl;
        cout << endl << "0 - BACK" << endl;
        do {
            test = checkinputchoice(input, 0, 0);
            if (test != 0&& test != 2)
                cerr << "Invalid option! Press 0 to go back." << endl;
        } while (test != 0 && test != 2);
        return;
    }
}

void Delegation::changeAthlete() {
    int test = 0;
    int index;
    string input = "", tmp;

    cout << "Name: ";
    getline(cin, tmp);
    if (cin.eof()) {
        cin.clear();
        return; //go back on ctrl+d
    }
    cin.clear();
    while (checkStringInput(tmp)) {
        cout << "Invalid Name. Try again!" << endl;
        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
    }
    index = findPerson(tmp);
    if (index == -1 || !(people.at(index)->isAthlete())) {
        throw NonExistentAthlete(tmp);
    } else {
        Date departure = people[index]->getDeparture();
        Date arrival = people[index]->getArrival();

        system("cls");
        cout << "_____________________________________________________" << endl << endl;
        cout << "\t\t   What do you want to change?" << endl;
        cout << "_____________________________________________________" << endl << endl;

        cout << "1 - Name" << endl;
        cout << "2 - Date of Birth" << endl;
        cout << "3 - Passport" << endl;
        cout << "4 - Date of Arrival" << endl;
        cout << "5 - Date of Departure" << endl;
        cout << "6 - Sport" << endl;
        cout << "7 - Weight" << endl;
        cout << "8 - Height" << endl;
        cout << "0 - BACK" << endl;

        do {
            test = checkinputchoice(input, 0, 8);
            if (test != 0 && test != 2)
                cerr << "Invalid option! Please try again." << endl;
        } while (test != 0 && test != 2);
        if (test == 2) { input = "0"; }

        Date tmp_date;
        int s_index;
        int t_index;
        vector<Competition> competitions;
        vector<string> competition_names;

        switch (stoi(input)) {
            case 1:
                cout << "New name: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkStringInput(tmp) || findPerson(tmp) != -1) {
                    cout << "Invalid Name. Try again!" << endl;
                    cout << "New name: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                people.at(index)->setName(tmp);
                athletes.at(index)->setName(tmp);
                break;
            case 2:
                cout << "Date of Birth: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkDateInput(tmp, tmp_date)) {
                    cout << "Invalid Date. Try again!" << endl;
                    cout << "Date of Birth: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                people.at(index)->setBirth(tmp_date);
                athletes.at(index)->setBirth(tmp_date);
                break;
            case 3:
                cout << "Passport: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkAlphaNumericInput(tmp)) {
                    cout << "Invalid Passport. Try again!" << endl;
                    cout << "Passport: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                people.at(index)->setPassport(tmp);
                athletes.at(index)->setPassport(tmp);
                break;
            case 4:
                cout << "Date of Arrival: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkDateInput(tmp, tmp_date) || !(tmp_date.isOlimpianDate()) || !(dateIsAfter(departure,tmp))) {
                    cout << "Invalid Date. Try again!" << endl;
                    cout << "Date of Arrival: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                people.at(index)->setArrival(tmp_date);
                athletes.at(index)->setArrival(tmp_date);
                break;
            case 5:
                cout << "Date of Departure: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkDateInput(tmp, tmp_date) || !(tmp_date.isOlimpianDate()) || !(dateIsBefore(arrival,tmp))) {
                    cout << "Invalid Date. Try again!" << endl;
                    cout << "Date of Departure: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                people.at(index)->setDeparture(tmp_date);
                athletes.at(index)->setDeparture(tmp_date);
                break;
            case 6:
                cout << "Sport: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkStringInput(tmp) || findSport(tmp) == -1) {
                    cout << "Invalid Sport. Try again!" << endl;
                    cout << "Sport: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                s_index = findSport(tmp);
                if(s_index == -1){
                    throw NonExistentSport(tmp);
                } else {
                    if (people.at(index)->isAthlete()) {
                        Athlete *a = dynamic_cast<Athlete *> (people.at(index));
                        if (a == NULL) {
                            cout << "Couldn't change Sport!" << endl;
                        } else {
                            athletes.at(index)->setSport(tmp);
                            a->setSport(tmp);

                            if (!(sports.at(s_index)->isTeamSport())) {
                                competitions = sports.at(s_index)->getCompetitions();
                                for (int i = 0; i < competitions.size(); i++) {
                                    competition_names.push_back(competitions.at(i).getName());
                                }
                                athletes.at(index)->setCompetitions(competition_names);
                                a->setCompetitions(competition_names);
                            } else {
                                TeamSport *ts = dynamic_cast<TeamSport *> (sports.at(s_index));
                                vector<Team *> tmp_teams = ts->getTeams();

                                for (int i = 0; i < tmp_teams.size(); i++) {
                                    cout << tmp_teams.at(i)->getName() << endl;
                                }

                                cout << "Team: ";
                                getline(cin, tmp);
                                if (cin.eof()) {
                                    cin.clear();
                                    return; //go back on ctrl+d
                                }
                                cin.clear();
                                while (checkAlphaNumericInput(tmp) == 1) {
                                    cout << "Invalid Team. Try again!" << endl;
                                    cout << "Team: ";
                                    getline(cin, tmp);
                                    if (cin.eof()) {
                                        cin.clear();
                                        return; //go back on ctrl+d
                                    }
                                    cin.clear();
                                }

                                t_index = findTeam(tmp);
                                if (t_index == -1) {
                                    throw NonExistentTeam(tmp);
                                } else {
                                    if ((teams.at(t_index)->getAthletes()).size() == ts->getNumberofElements()) {
                                        throw FullTeam(tmp);
                                    } else {
                                        string t = "";

                                        for(int i = 0; i < teams.size(); i++){
                                            vector<Athlete*> tmp_athletes = teams.at(i)->getAthletes();
                                            for(int j = 0; j < tmp_athletes.size(); j++){
                                                if (tmp_athletes.at(j)->getName() == a->getName()) t = teams.at(i)->getName();
                                            }
                                        }

                                        competition_names = teams.at(t_index)->getCompetitions();
                                        athletes.at(index)->setCompetitions(competition_names);
                                        a->setCompetitions(competition_names);
                                        teams.at(t_index)->addAthlete(a);

                                        if(t != "") {
                                            t_index = findTeam(t);
                                            teams.at(t_index)->removeAthlete(a->getName());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case 7:
                cout << "Weight: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkFloatInput(tmp)) {
                    cerr << "Invalid Weight. Try again!" << endl;
                    cout << "Weight: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                if (people.at(index)->isAthlete()) {
                    Athlete *a = dynamic_cast<Athlete *> (people.at(index));
                    if (a == NULL) {
                        cout << "Couldn't change Weight!" << endl;
                    } else {
                        athletes.at(index)->setWeight(stof(tmp));
                        a->setWeight(stof(tmp));
                    }
                }
                break;
            case 8:
                cout << "Height: ";
                getline(cin, tmp);
                if (cin.eof()) {
                    cin.clear();
                    return; //go back on ctrl+d
                }
                cin.clear();
                while (checkPositiveIntInput(tmp)) {
                    cerr << "Invalid Height. Try again!" << endl;
                    cout << "Height: ";
                    getline(cin, tmp);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                }
                if (people.at(index)->isAthlete()) {
                    Athlete *a = dynamic_cast<Athlete *> (people.at(index));
                    if (a == NULL) {
                        cout << "Couldn't change Height!" << endl;
                    } else {
                        athletes.at(index)->setHeight(stof(tmp));
                        a->setHeight(stoi(tmp));
                    }
                }
                break;
            case 0:
                break;
            default:
                break;
        }

        cout << endl << "Athlete changed with success!" << endl;
        cout << endl << "0 - BACK" << endl;
        do {
            test = checkinputchoice(input, 0, 0);
            if (test != 0&& test != 2)
                cerr << "Invalid option! Press 0 to go back." << endl;
        } while (test != 0 && test != 2);
    }
}

void Delegation::showAthlete() const {
    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(21) << " "<<"Information about an Athlete" << endl;
    cout << "----------------------------------------------------------------------" << endl<<endl;


    if (!people.empty()) {
        int test = 0;
        int index;
        string input = "", tmp;

        cout << "Name: ";
        getline(cin, tmp);
        if (cin.eof()) {
            cin.clear();
            return; //go back on ctrl+d
        }
        cin.clear();
        while (checkStringInput(tmp)) {
            cout << "Invalid Name. Try again!" << endl;
            cout << "Name: ";
            getline(cin, tmp);
            if (cin.eof()) {
                cin.clear();
                return; //go back on ctrl+d
            }
            cin.clear();
        }
        index = findPerson(tmp);
        if (index == -1 || !people.at(index)->isAthlete())
            throw NonExistentAthlete(tmp);
        else {
            cout << endl;
            (*(people.begin() + index))->showInfo();
            if((*(people.begin() + index))->playsTeamSport(*this)){
                for(size_t i=0; i<teams.size();i++){
                    vector<Athlete*> ats=teams[i]->getAthletes();
                    for(size_t j=0;j<ats.size(); j++){
                        if(ats[j]->getName() == (*(people.begin() + index))->getName())
                            cout <<  left <<setw(17) << "Team" <<  setw(4) << " " << teams[i]->getName() <<endl;
                    }
                }
            }
        }
    } else
        throw NoMembers();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showAllAthletes() {
    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(22) << " " <<"Information about Athletes" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;


    if (!athletes.empty()) {
        vector<Athlete *>::const_iterator it;
        for (it = athletes.begin(); it != athletes.end(); it++) {
            (*it)->showInfo();
            if((*it)->playsTeamSport(*this)){
                for(size_t i=0; i<teams.size();i++){
                    vector<Athlete*> ats=teams[i]->getAthletes();
                    for(size_t j=0;j<ats.size(); j++){
                        if(ats[j]->getName() == (*it)->getName())
                            cout <<  left <<setw(17) << "Team" <<  setw(4) << " " << teams[i]->getName() <<endl;
                    }
                }
            }
            cout << endl;
        }
    } else
        throw NoMembers();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

//Teams Functions

void Delegation::showTeam() const {
    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------"  << endl;
    cout << setw(23) << " " <<"Information about a Team" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;


    if (!teams.empty()) {
        int test = 0;
        int index;
        string input = "", nm;
        bool found = false;

        do {
            cout << "Name: ";
            getline(cin, nm);
            if (cin.eof()) {
                cin.clear();
                return; //go back on ctrl+d
            }
            cin.clear();
        } while (cin.fail());


        vector<Team *>::const_iterator t;
        for (t = teams.begin(); t != teams.end(); t++) {
            if ((*t)->getName() == nm) {
                (*t)->showInfo();
                found = true;
            }
        }
        if (!found)
            throw NonExistentTeam(nm);

    } else
        throw NoMembers();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showAllTeams() {
    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(23) << " " <<"Information about Teams" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;


    if (!teams.empty()) {
        sort(teams.begin(), teams.end(), sortMembersAlphabetically<Team>);
        vector<Team *>::const_iterator it;
        for (it = teams.begin(); it != teams.end(); it++) {
            (*it)->showInfo();
        }
    } else
        throw NoMembers();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

int Delegation::findTeam(const string &name) const {
    for (int i = 0; i < teams.size(); i++) {
        if (name == teams.at(i)->getName()) return i;
    }
    return -1;
}

//Sports Functions

void Delegation::showCompetition(const string & sport){
    int test = 0;
    string input = "";
    int testinput=0;
    vector<Competition>::const_iterator cit;

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    int space = (70-(sport.size()+17))/2;
    cout << setw(space) << " " << sport<<" Competition Info" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    vector<Sport * >::iterator s = sports.begin();
    string nm;

    while(s!= sports.end()){
        if((*s)->getName() == sport){
            vector<Competition> c = (*s)->getCompetitions();
            if(!c.empty()){
                bool found = false;

                do {
                    cout << "Competition's name: ";
                    getline(cin, nm);
                    if (cin.eof()) {
                        cin.clear();
                        return; //go back on ctrl+d
                    }
                    cin.clear();
                } while (cin.fail());

                for (cit = c.begin(); cit != c.end(); cit++) {
                    if (cit->getName() == nm) {
                        cout << endl;
                        cit->showInfo();
                        found = true;
                        break;
                    }
                }
                if (!found)
                    throw NonExistentCompetition(nm,sport);
            }
            else
                throw NoCompetitions(sport);
        }
        s++;
    }

    cout << endl << "1 - Trials Details";
    cout << endl << "0 - BACK" << endl;

    do {
        test = checkinputchoice(input, 0, 1);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
    if (testinput == 2)
    { input = "0"; }

    switch (stoi(input)) {
        case 1:
            try{
                showTrials(nm,sport);
            }
            catch(NoTrials & t){
                cout << t;
                exceptionHandler();
            }
            break;
        case 0:
            break;
    }
}

void Delegation::showAllCompetitions(const string & sport){
    int test = 0;
    string input = "";
    vector<Competition> competitions;

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    int space = (70-(sport.size()+13))/2;
    cout << setw(space) << " " << sport << " Competitions" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    for(size_t i=0; i< sports.size(); i++){
        if(sport == sports[i]->getName()){
            competitions = sports[i]->getCompetitions();
            break;
        }
    }
    if (!competitions.empty()) {
        sort(competitions.begin(), competitions.end(), sortCompetitionsByDate);
        vector<Competition>::const_iterator it;
        for (it = competitions.begin(); it != competitions.end(); it++) {
            it->showInfo();
            cout << endl;
        }
    } else
        throw NoCompetitions(sport);

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showAllTrials(const string & sport){
    int test = 0;
    string input = "";
    vector<Competition> competitions;
    int nTrials=0;

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    int space = (70-(sport.size()+7))/2;
    cout << setw(space) << " " << sport << " Trials" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    for(size_t i=0; i< sports.size(); i++){
        if(sport == sports[i]->getName()){
            competitions = sports[i]->getCompetitions();
            break;
        }
    }
    if (!competitions.empty()) {
        sort(competitions.begin(), competitions.end(), sortCompetitionsByDate);
        vector<Competition>::const_iterator it;
        for (it = competitions.begin(); it != competitions.end(); it++) {
            vector<Trial> trials = it->getTrials();
            if(!trials.empty()){
                cout << it->getName()<< " competition:" << endl;
                for(size_t i=0; i< trials.size(); i++){
                    trials[i].showInfo();
                    cout << endl;
                }
                nTrials++;
            }
        }
        if(nTrials == 0)
            throw NoTrials(sport);
    } else
        throw NoCompetitions(sport);

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showAllTrials(){
    int test = 0;
    string input = "";
    vector<Trial> allTrials;
    vector<Competition>::const_iterator it;

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(27) << " "<<"Trials Calendar" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    if (!sports.empty()) {
        for (size_t i = 0; i < sports.size(); i++) {
            vector<Competition> competitions;
            competitions = sports[i]->getCompetitions();
            if (!competitions.empty()) {
                sort(competitions.begin(), competitions.end(), sortCompetitionsByDate);
                for (it = competitions.begin(); it != competitions.end(); it++) {
                    vector<Trial> trials = it->getTrials();
                    if (!trials.empty()){
                        allTrials.insert(allTrials.end(),trials.begin(),trials.end());
                    }
                }
            }
        }
        if(allTrials.empty())
            throw NoTrials();
    } else throw NoSports();

    sort(allTrials.begin(),allTrials.end(),sortTrialsByDate);
    vector<Date> dates;

    for(size_t i=0; i< allTrials.size(); i++){
        if(i==0) dates.push_back(allTrials[i].getDate());
        else{
            if(find(dates.begin(),dates.end(),allTrials[i].getDate()) == dates.end())
                dates.push_back(allTrials[i].getDate());
        }
    }

    int tmp=0;
    for(size_t j=0; j<dates.size();j++){
        cout << "--------------"<<endl;
        cout << "  " << dates[j] <<endl;
        cout << "--------------"<<endl;
        for(size_t i=tmp; i< allTrials.size(); i++){
            if(allTrials.at(i).getDate() == dates[j]){
                allTrials[i].showInfoNoDate();
                cout <<endl;
            }
            else{
                tmp=i;
                break;
            }
        }
    }

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showTrials(const string & comp,const string & sport) const{
    int test = 0;
    string input = "";
    vector<Competition> competitions;

    for(size_t i=0; i< sports.size(); i++){
        if(sport == sports[i]->getName()){
            competitions = sports[i]->getCompetitions();
            break;
        }
    }
    if (!competitions.empty()) {
        sort(competitions.begin(), competitions.end(), sortCompetitionsByDate);
        vector<Competition>::const_iterator it;
        for (it = competitions.begin(); it != competitions.end(); it++) {
            if(it->getName() == comp){
                vector<Trial> trials = it->getTrials();
                sort(trials.begin(),trials.end(),sortTrialsByDate);
                if(!trials.empty()){
                    cout << it->getName()<< " trials:" << endl;
                    for(size_t i=0; i< trials.size(); i++){
                        trials[i].showInfo();
                        cout << endl;
                    }
                    cout << endl << "0 - BACK" << endl;
                    do {
                        test = checkinputchoice(input, 0, 0);
                        if (test != 0 && test != 2)
                            cerr << "Invalid option! Press 0 to go back." << endl;
                    } while (test != 0 && test != 2);
                    return;
                }
                else
                    throw NoTrials(comp);
            }
        }
    } else
        throw NoCompetitions(sport);

}

void Delegation::showTrialsInDay(){
    int test = 0;
    string input = "";
    vector<Trial> allTrials;
    vector<Competition>::const_iterator it;

    system("cls");
    cout << "----------------------------------------------------------------------"<< endl;
    cout << setw(27) << " "<<"Trials Calendar" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    if (!sports.empty()) {
        for (size_t i = 0; i < sports.size(); i++) {
            vector<Competition> competitions;
            competitions = sports[i]->getCompetitions();
            if (!competitions.empty()) {
                sort(competitions.begin(), competitions.end(), sortCompetitionsByDate);
                for (it = competitions.begin(); it != competitions.end(); it++) {
                    vector<Trial> trials = it->getTrials();
                    if (!trials.empty()){
                        allTrials.insert(allTrials.end(),trials.begin(),trials.end());
                    }
                }
            }
        }
        if(allTrials.empty())
            throw NoTrials();
    } else throw NoSports();

    sort(allTrials.begin(),allTrials.end(),sortTrialsByDate);
    vector<Date> dates;

    for(size_t i=0; i< allTrials.size(); i++){
        if(i==0) dates.push_back(allTrials[i].getDate());
        else{
            if(find(dates.begin(),dates.end(),allTrials[i].getDate()) == dates.end())
                dates.push_back(allTrials[i].getDate());
        }
    }

    string date;
    Date d;
    bool valid =false;
    do{
        cout << "Date:";
        getline(cin,date);
        if(cin.eof()){
            cin.clear();
            return;
        }
        if(checkDateInput(date,d))
            cerr << "Invalid date, please try again!"<<endl;
        else{
            if (!d.isOlimpianDate())
                cerr << "Invalid date, please try again!"<<endl;
            else
                valid=true;
        }
        }while(checkDateInput(date,d) || !valid);

    bool noTrialsInDay=true;
    cout<<endl;
    cout << "--------------"<<endl;
    cout << "  " << d <<endl;
    cout << "--------------"<<endl;
    for(size_t i=0; i< allTrials.size(); i++){
        if(allTrials[i].getDate() == d){
            noTrialsInDay=false;
            allTrials[i].showInfoNoDate();
            cout <<endl;
        }
    }


    if(noTrialsInDay)
        cout << "No Trials occur in that Day!"<<endl;

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

int Delegation::findSport(const string & name) const {
    for (int i = 0; i < sports.size(); i++) {
        if (name == sports.at(i)->getName()) return i;
    }
    return -1;
}

//Sort People
void Delegation::sortAllPeople() {
    sort(people.begin(), people.end(), sortPersons);
    sort(athletes.begin(), athletes.end(), sortPersons);
}

//Medals Functions
void Delegation::showAllMedals() const {
    int test = 0;
    string input = "",sport="";

    system("cls");
    cout << "----------------------------------------------------------------------"<< endl;
    cout << setw(30) << " "<<"All Medals" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator c;
    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            sort(comps.begin(), comps.end(),sortCompetitionsByDate);
            if(!comps.empty()) {
                sport = (*s)->getName();
                transform(sport.begin(), sport.end(), sport.begin(), ::toupper);
                cout <<sport<<endl;
                for (c = comps.begin(); c != comps.end(); c++) {
                    cout << c->getName()<<endl;
                    c->showMedals();
                    cout << endl;

                }
            }
        }
    } else
        throw NoMedals();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

//Most awarded countries(total medals)
void Delegation::showCountryMedals() const{
    int test = 0;
    string input = "",sport="", cntr;
    bool noMedals = true,noMedalsInComp,noMedalsInSport;
    int g=0,sil=0,b=0;

    cout << "Country: ";
    do{
        getline(cin,cntr);
        if(cin.eof()){
            cin.clear();
            return;
        }
        if(checkStringInput(cntr))
            cerr << "Invalid number, please try again!"<<endl;
    }while(checkStringInput(cntr));

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    int space = (70-(sport.size()+7))/2;
    cout << setw(space)<<" " << country <<  " Medals " << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator c;
    vector<Medal>::iterator m;

    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            if(!comps.empty()) {
                noMedalsInSport = true;
                for (c = comps.begin(); c != comps.end(); c++) {
                    vector<Medal>medals = c->getMedals();
                    insertionSort(medals);
                    noMedalsInComp = true;
                    for(m=medals.begin(); m!=medals.end();m++){
                        if(caseInSensStringCompare(m->getCountry(),cntr)){
                            if(noMedalsInSport){
                                sport = (*s)->getName();
                                transform(sport.begin(), sport.end(), sport.begin(), ::toupper);
                                cout <<sport<<endl;
                                noMedalsInSport=false;
                            }
                            if(noMedalsInComp){
                                noMedalsInComp = false;
                               cout << c->getName()<<endl;
                            }
                            m->showInfo();
                            noMedals = false;
                            //count number of medals
                            if(m->getType() == 'g')
                                g++;
                            else if(m->getType() == 's')
                                sil++;
                            else
                                b++;
                        }
                    }
                    if(!noMedalsInComp)
                        cout << endl;
                }
            }
        }
        if(noMedals)
            throw NoMedals(cntr);
    } else
        throw NoMedals();

    cout<< country << " won "<< g+sil+b << " medals: " <<endl;
    cout << left << setw(18) << "->Gold Medals "<<g<<endl;
    cout << left << setw(18)<< "->Silver Medals "<<sil<<endl;
    cout << left << setw(18)<< "->Bronze Medals "<<b<<endl;

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showCountryMedals(const string & c) const{
    int test = 0;
    string input = "",sport="";
    bool noMedals = true,noMedalsInComp,noMedalsInSport;
    int g=0,sil=0,b=0;

    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator cit;
    vector<Medal>::iterator m;

    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            if(!comps.empty()) {
                noMedalsInSport = true;
                for (cit = comps.begin(); cit != comps.end(); cit++) {
                    vector<Medal>medals = cit->getMedals();
                    insertionSort(medals);
                    noMedalsInComp = true;
                    for(m=medals.begin(); m!=medals.end();m++){
                        if(caseInSensStringCompare(m->getCountry(),c)){
                            if(noMedalsInSport){
                                sport = (*s)->getName();
                                transform(sport.begin(), sport.end(), sport.begin(), ::toupper);
                                cout <<sport<<endl;
                                noMedalsInSport=false;
                            }
                            if(noMedalsInComp){
                                noMedalsInComp = false;
                                cout << cit->getName()<<endl;
                            }
                            m->showInfo();
                            //count number of medals
                            if(m->getType() == 'g')
                                g++;
                            else if(m->getType() == 's')
                                sil++;
                            else
                                b++;
                        }
                    }
                    if(!noMedalsInComp)
                        cout << endl;
                }
            }
        }
    }

    cout<< country << " won "<< g+sil+b << " medals: " <<endl;
    cout << left << setw(18) << "->Gold Medals "<<g<<endl;
    cout << left << setw(18)<< "->Silver Medals "<<sil<<endl;
    cout << left << setw(18)<< "->Bronze Medals "<<b<<endl;

}

int Delegation::numberOfMedalsCountry(const string & name) const{
    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator c;
    vector<Medal>::iterator m;
    bool noMedals=true;
    int n=0;

    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            if(!comps.empty()) {
                for (c = comps.begin(); c != comps.end(); c++) {
                    vector<Medal>medals = c->getMedals();
                    insertionSort(medals);
                    for(m=medals.begin(); m!=medals.end();m++){
                        if(caseInSensStringCompare(m->getCountry(),name)){
                            n++;
                            noMedals =false;
                        }
                    }
                }
            }
        }
        if(noMedals)
            throw NoMedals(country);
    } else
        throw NoMedals();

    return n;
}

vector<string> Delegation::getCountriesWithMedals() const{
    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator c;
    vector<Medal>::iterator m;

    vector<string> countries;
    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            if(!comps.empty()) {
                for (c = comps.begin(); c != comps.end(); c++) {
                    vector<Medal>medals = c->getMedals();
                    insertionSort(medals);
                    for(m=medals.begin(); m!=medals.end();m++){
                        countries.push_back(m->getCountry());
                    }
                }
            }
        }
    } else
        throw NoMedals();

    noRepeatVector(countries);
    return countries;
}

void countriesSort(const Delegation & d,vector<string> & countries){
    for (unsigned int p = 1; p < countries.size(); p++)
    {
        string tmp = countries[p];
        int j;
        for (j = p; j > 0 && d.compareCountriesByMedals(tmp,countries[j-1]); j--)
            countries[j] = countries[j-1];
        countries[j] = tmp;
    }
}

void Delegation::mostAwardedCountries() const{
    int test = 0,nMax;
    string input = "",n="",temp="";
    vector<string> countries;
    bool notValid=false;
    int g=0,sil=0,b=0;

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(24) << " "<<"Most Awarded Countries" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    try{
        countries = getCountriesWithMedals();
    }catch(NoMedals & e){throw;}

    countriesSort((*this),countries);
    nMax=countries.size();

    cout << "Show countries ranking positions from 1st to nth positions."<<endl;
    cout<< "Choose n from 1 to " << nMax << ": ";

    do{
        getline(cin,n);
        if(cin.eof()){
            cin.clear();
            return;
        }
        if(checkPositiveIntInput(n))
            cerr << "Invalid number, please try again!"<<endl;
        else
            notValid = stoi(n) < 1 || stoi(n) > nMax;
        if(notValid) cerr << "Invalid number, please try again!"<<endl;
    }while(checkPositiveIntInput(n) || notValid);


    cout<< endl<< left << setw(4) << " " << setw(10) <<  "COUNTRY" << left << setw(4) << "|"<< setw(4) << "MEDALS" <<endl;
    cout << "--------------|----------"<<endl;
    for(size_t i= 0; i<stoi(n); i++){
        cout<< left << setw(4) << " " << setw(10)<<  countries[i] << left << setw(4) << "|"<< setw(4) << (*this).numberOfMedalsCountry(countries[i]) << endl;
    }
    cout << "--------------|----------"<<endl<<endl;
    cout << endl << "1 - Medals Details";
    cout << endl << "0 - BACK" << endl;

    do {
        test = checkinputchoice(input, 0, 1);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
    if (test == 2)
    { input = "0"; }

    switch (stoi(input)) {
        case 1:
            cout << endl;
            for (size_t i = 0; i < stoi(n); i++) {
                temp = countries[i];
                transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
                for (int j = 0; j < temp.size() * 3; j++)cout << "-";
                cout << endl;
                for (int j = 0; j < temp.size(); j++)cout << " ";
                cout << temp << endl;
                for (int j = 0; j < temp.size() * 3; j++)cout << "-";
                cout << endl;
                showCountryMedals(countries[i]);
                cout << endl;
            }
            cout << endl << "0 - BACK" << endl;
            do {
                test = checkinputchoice(input, 0, 0);
                if (test != 0&& test != 2)
                    cerr << "Invalid option! Press 0 to go back." << endl;
            } while (test != 0 && test != 2);
            break;
        case 0:
            break;
    }
}

bool Delegation::compareCountriesByMedals(const string &left, const string & right) const{
    return numberOfMedalsCountry(left) > numberOfMedalsCountry(right);
}
//Most awarded countries(gold medals)
void Delegation::mostAwardedGold() const{
    int test = 0,nMax;
    string input = "",n="",temp="";
    vector<string> countries;
    bool notValid=false;
    int g=0,sil=0,b=0;

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(17) << " " <<"Most Awarded Countries - Gold Medals" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    try{
        countries = getCountriesWithGoldMedals();
    }catch(NoMedals & e){throw;}

    countriesSortGold((*this),countries);
    nMax=countries.size();

    cout << "Show countries ranking positions from 1st to nth positions."<<endl;
    cout<< "Choose n from 1 to " << nMax << ": ";

    do{
        getline(cin,n);
        if(cin.eof()){
            cin.clear();
            return;
        }
        if(checkPositiveIntInput(n))
            cerr << "Invalid number, please try again!"<<endl;
        else
            notValid = stoi(n) < 1 || stoi(n) > nMax;
        if(notValid) cerr << "Invalid number, please try again!"<<endl;
    }while(checkPositiveIntInput(n) || notValid);


    cout<< endl<< left << setw(4) << " " << setw(10) <<  "COUNTRY" << left << setw(4) << "|"<< setw(4) << "GOLD MEDALS" <<endl;
    cout << "--------------|--------------"<<endl;
    for(size_t i= 0; i<stoi(n); i++){
        cout<< left << setw(4) << " " << setw(10)<<  countries[i] << left << setw(4) << "|"<< setw(4) << (*this).numberOfGoldMedalsCountry(countries[i]) << endl;
    }
    cout << "--------------|--------------"<<endl<<endl;

    cout << endl << "1 - Medals Details";
    cout << endl << "0 - BACK" << endl;

    do {
        test = checkinputchoice(input, 0, 1);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
    if (test == 2)
    { input = "0"; }

    switch (stoi(input)) {
        case 1:
            cout << endl;
            for (size_t i = 0; i < stoi(n); i++) {
                temp = countries[i];
                transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
                for (int j = 0; j < temp.size() * 3; j++)cout << "-";
                cout << endl;
                for (int j = 0; j < temp.size(); j++)cout << " ";
                cout << temp << endl;
                for (int j = 0; j < temp.size() * 3; j++)cout << "-";
                cout << endl;
                showCountryGoldMedals(countries[i]);
            }
            cout << endl << "0 - BACK" << endl;
            do {
                test = checkinputchoice(input, 0, 0);
                if (test != 0&& test != 2)
                    cerr << "Invalid option! Press 0 to go back." << endl;
            } while (test != 0 && test != 2);
            break;
        case 0:
            break;
    }
}

vector<string> Delegation::getCountriesWithGoldMedals() const{
    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator c;
    vector<Medal>::iterator m;

    vector<string> countries;
    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            if(!comps.empty()) {
                for (c = comps.begin(); c != comps.end(); c++) {
                    vector<Medal>medals = c->getMedals();
                    insertionSort(medals);
                    for(m=medals.begin(); m!=medals.end();m++){
                        if(m->getType() == 'g')
                            countries.push_back(m->getCountry());
                    }
                }
            }
        }
    } else
        throw NoMedals();

    noRepeatVector(countries);
    return countries;
}

bool Delegation::compareCountriesByGoldMedals(const string &left, const string & right) const{
    return numberOfGoldMedalsCountry(left) > numberOfGoldMedalsCountry(right);
}

void countriesSortGold(const Delegation & d,vector<string> & countries){
    for (unsigned int p = 1; p < countries.size(); p++)
    {
        string tmp = countries[p];
        int j;
        for (j = p; j > 0 && d.compareCountriesByGoldMedals(tmp,countries[j-1]); j--)
            countries[j] = countries[j-1];
        countries[j] = tmp;
    }
}

int Delegation::numberOfGoldMedalsCountry(const string & name) const{
    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator c;
    vector<Medal>::iterator m;
    bool noMedals=true;
    int n=0;

    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            if(!comps.empty()) {
                for (c = comps.begin(); c != comps.end(); c++) {
                    vector<Medal>medals = c->getMedals();
                    insertionSort(medals);
                    for(m=medals.begin(); m!=medals.end();m++){
                        if(caseInSensStringCompare(m->getCountry(),name) && m->getType() == 'g'){
                            n++;
                            noMedals =false;
                        }
                    }
                }
            }
        }
        if(noMedals)
            throw NoMedals(country);
    } else
        throw NoMedals();

    return n;
}

void Delegation::showCountryGoldMedals(const string & c) const{
    int test = 0;
    string input = "",sport="";
    bool noMedalsInComp,noMedalsInSport;

    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator cit;
    vector<Medal>::iterator m;

    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            if(!comps.empty()) {
                noMedalsInSport = true;
                for (cit = comps.begin(); cit != comps.end(); cit++) {
                    vector<Medal>medals = cit->getMedals();
                    insertionSort(medals);
                    noMedalsInComp = true;
                    for(m=medals.begin(); m!=medals.end();m++){
                        if(caseInSensStringCompare(m->getCountry(),c) && m->getType() == 'g'){
                            if(noMedalsInSport){
                                sport = (*s)->getName();
                                transform(sport.begin(), sport.end(), sport.begin(), ::toupper);
                                cout << sport<<endl;
                                noMedalsInSport=false;
                            }
                            if(noMedalsInComp){
                                noMedalsInComp = false;
                                cout << left << setw(23) <<cit->getName();
                            }
                            cout << left << m->getWinner();

                        }
                    }
                    if(!noMedalsInComp)
                        cout << endl;
                }
                if(!noMedalsInComp)
                    cout << endl;
            }
        }
    }
}

//Most awarded athletes
void Delegation::showAthleteMedals(const string & c) const{
    int test = 0;
    string input = "",sport="";
    bool noMedals = true,noMedalsInComp,noMedalsInSport;
    int g=0,sil=0,b=0;
    string nm;

    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator cit;
    vector<Medal>::iterator m;

    if (!sports.empty()) {
        vector<Sport *>sp = sports;
        sort(sp.begin(), sp.end(),sortMembersAlphabetically<Sport>);
        for(s = sp.begin(); s != sp.end(); s++){
            vector<Competition>comps = (*s)->getCompetitions();
            if(!comps.empty()) {
                noMedalsInSport = true;
                for (cit = comps.begin(); cit != comps.end(); cit++) {
                    vector<Medal>medals = cit->getMedals();
                    insertionSort(medals);
                    noMedalsInComp = true;
                    for(m=medals.begin(); m!=medals.end();m++){
                        if((*s)->isTeamSport()) nm=getAthleteTeam(c)->getName();
                        else nm=c;
                        if(caseInSensStringCompare(m->getWinner(),nm)){
                            if(noMedalsInSport){
                                sport = (*s)->getName();
                                transform(sport.begin(), sport.end(), sport.begin(), ::toupper);
                                cout <<sport<<endl;
                                noMedalsInSport=false;
                            }
                            if(noMedalsInComp){
                                noMedalsInComp = false;
                                cout << left << setw(15) << cit->getName();
                            }
                            m->showType();
                            //count number of medals
                            if(m->getType() == 'g')
                                g++;
                            else if(m->getType() == 's')
                                sil++;
                            else
                                b++;
                        }
                    }
                    if(!noMedalsInComp)
                        cout << endl;
                }
            }
        }
    }

    cout<< c << " won "<< g+sil+b << " medals: " <<endl;
    cout << left << setw(18) << "->Gold Medals "<<g<<endl;
    cout << left << setw(18)<< "->Silver Medals "<<sil<<endl;
    cout << left << setw(18)<< "->Bronze Medals "<<b<<endl;
}

int Delegation::numberOfMedalsAthlete(const string & name) const{
    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator c;
    vector<Medal>::iterator m;
    bool noMedals = true;
    vector<string> ats;
    int n=0;

    //verifica se os atletas têm medalhas em desportos individuais e, se tiverem, adiciona-os a outro vetor
    if (!sports.empty()) {
        vector<Sport *> sp = sports;
        sort(sp.begin(), sp.end(), sortMembersAlphabetically<Sport>);
        for (s = sp.begin(); s != sp.end(); s++) {
            if (!(*s)->isTeamSport()) {
                vector<Competition> comps = (*s)->getCompetitions();
                if (!comps.empty()) {
                    for (c = comps.begin(); c != comps.end(); c++) {
                        vector<Medal> medals = c->getMedals();
                        insertionSort(medals);
                        for (m = medals.begin(); m != medals.end(); m++) {
                            if (m->getWinner() == name)
                                n++;
                        }
                    }
                }
                noMedals = false;
            }else{
                Team * t = getAthleteTeam(name);
                vector<Competition> comps = (*s)->getCompetitions();
                if (!comps.empty()) {
                    for (c = comps.begin(); c != comps.end(); c++) {
                        vector<Medal> medals = c->getMedals();
                        insertionSort(medals);
                        for (m = medals.begin(); m != medals.end(); m++) {
                            if (t->getName() == m->getWinner())
                                n++;
                        }
                    }
                }
                noMedals = false;
            }
        }
        if (noMedals)
            throw NoMedals();
    } else
        throw NoMedals();

    return n;
}

vector<string> Delegation::getAthletesWithMedals() const{
    vector<Sport*>::const_iterator s;
    vector<Competition>::iterator c;
    vector<Medal>::iterator m;
    bool noMedals = true;
    vector<string> ats;
    vector<string> aWithMedals;

    //costrói vetor de strings com todos os atletas
    for(size_t i=0; i< athletes.size();i++)
        ats.push_back(athletes[i]->getName());

    //verifica se os atletas têm medalhas em desportos individuais e, se tiverem, adiciona-os a outro vetor
    for(size_t a=0; a < ats.size(); a++) {
        if (!sports.empty()) {
            vector<Sport *> sp = sports;
            sort(sp.begin(), sp.end(), sortMembersAlphabetically<Sport>);
            for (s = sp.begin(); s != sp.end(); s++) {
                if (!(*s)->isTeamSport()) {
                    vector<Competition> comps = (*s)->getCompetitions();
                    if (!comps.empty()) {
                        for (c = comps.begin(); c != comps.end(); c++) {
                            vector<Medal> medals = c->getMedals();
                            insertionSort(medals);
                            for (m = medals.begin(); m != medals.end(); m++) {
                                if (ats[a] == m->getWinner())
                                    aWithMedals.push_back(ats[a]);
                            }
                        }
                    }
                    noMedals = false;
                }
                else{
                    Team * t = getAthleteTeam(ats[a]);
                    vector<Competition> comps = (*s)->getCompetitions();
                    if (!comps.empty()) {
                        for (c = comps.begin(); c != comps.end(); c++) {
                            vector<Medal> medals = c->getMedals();
                            insertionSort(medals);
                            for (m = medals.begin(); m != medals.end(); m++) {
                                if (t->getName() == m->getWinner())
                                    aWithMedals.push_back(ats[a]);
                            }
                        }
                    }
                    noMedals = false;
                }
            }
            if (noMedals)
                throw NoMedals();
        } else
            throw NoMedals();
    }

    noRepeatVector(aWithMedals);
    return aWithMedals;
}

Team * Delegation::getAthleteTeam(const string & at) const{
    //find athletes team
    bool found=false;
    Team * emptyTeam = new Team();

    for(size_t j=0; j<teams.size();j++){
        vector<Athlete*> participants=teams[j]->getAthletes();
        for(size_t i=0;i<participants.size(); i++){
            if(participants[i]->getName() == at)
               found=true;
            break;
        }
        if(found) return teams[j];
    }
    return emptyTeam;
}

bool Delegation::compareAthletesByMedals(const string &left, const string & right) const{
    return numberOfMedalsAthlete(left) > numberOfMedalsAthlete(right);
}

void athletesSort(const Delegation & d,vector<string> & ats){
    for (unsigned int p = 1; p < ats.size(); p++)
    {
        string tmp = ats[p];
        int j;
        for (j = p; j > 0 && d.compareAthletesByMedals(tmp,ats[j-1]); j--)
            ats[j] = ats[j-1];
        ats[j] = tmp;
    }
}

void Delegation::mostAwardedAthletes() const{
    int test = 0,nMax;
    string input = "",n="",temp="";
    vector<string> ats;
    bool notValid=false;
    int g=0,sil=0,b=0;

    system("cls");
    cout << "----------------------------------------------------------------------"  << endl;
    cout << setw(24) << " " <<"Most Awarded Atheltes" << endl;
    cout << "----------------------------------------------------------------------" << endl << endl;

    try{
        ats = getAthletesWithMedals();
    }catch(NoMedals & e){throw;}
    athletesSort((*this),ats);
    nMax=ats.size();

    cout << "Show athletes ranking positions from 1st to nth positions."<<endl;
    cout<< "Choose n from 1 to " << nMax << ": ";

    do{
        getline(cin,n);
        if(cin.eof()){
            cin.clear();
            return;
        }
        if(checkPositiveIntInput(n))
            cerr << "Invalid number, please try again!"<<endl;
        else
            notValid = stoi(n) < 1 || stoi(n) > nMax;
        if(notValid) cerr << "Invalid number, please try again!"<<endl;
    }while(checkPositiveIntInput(n) || notValid);


    cout<< endl<< left << setw(4) << " " << setw(20) <<  "ATHLETE" << left << setw(4) << "|"<< setw(4) << "MEDALS" <<endl;
    cout << "------------------------|----------"<<endl;
    for(size_t i= 0; i<stoi(n); i++){
        cout<< left << setw(4) << " " << setw(20)<<  ats[i] << left << setw(4) << "|"<< setw(4) << (*this).numberOfMedalsAthlete(ats[i]) << endl;
    }
    cout << "------------------------|----------"<<endl<<endl;
    cout << endl << "1 - Medals Details";
    cout << endl << "0 - BACK" << endl;

    do {
        test = checkinputchoice(input, 0, 1);
        if (test != 0 && test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
    if (test == 2)
    { input = "0"; }

    switch (stoi(input)) {
        case 1:
            cout << endl;
            for (size_t i = 0; i < stoi(n); i++) {
                temp = ats[i];
                transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
                for (int j = 0; j < temp.size() * 3; j++)cout << "-";
                cout << endl;
                for (int j = 0; j < temp.size(); j++)cout << " ";
                cout << temp << endl;
                for (int j = 0; j < temp.size() * 3; j++)cout << "-";
                cout << endl;
                showAthleteMedals(ats[i]);
                cout << endl;
            }
            cout << endl << "0 - BACK" << endl;
            do {
                test = checkinputchoice(input, 0, 0);
                if (test != 0&& test != 2)
                    cerr << "Invalid option! Press 0 to go back." << endl;
            } while (test != 0 && test != 2);
            break;
        case 0:
            break;
    }
}

//Records (BST) functions
bool newRecord(float result,float record,char comparisonCriteria, string place){
    if(result == -1) return false; //se o jogo ainda não tem resultado

    if(record == -1) return true; //se ainda não existe recorde para uma certa competição/jogo e há dados do resultado nos jogos de tóquio

    if(place == "Tokyo") return true;

    if(comparisonCriteria == '+'){ //se existem dados do recorde mundial e dos jogos de Tóquio
        return (result > record);
    }
    return result < record;
}

void Delegation::setRecords(){
    vector<Sport*>::iterator s;
    Record notFound;

    for(s=sports.begin(); s!= sports.end(); s++){
        if(!(*s)->isTeamSport()){ //se for um desporto individual
            vector<Competition> competitions = (*s)->getCompetitions();
            vector<Competition>::iterator c;
            for(c=competitions.begin();c!=competitions.end();c++){ //verifica se alguma das competições existe nos recordes
                Record r((*s)->getName(),c->getName());
                Record foundOrNot = records.find(r);
                if(foundOrNot == notFound){//verifica se os trials da competição existem nos recordes se não existir apenas a competição
                    vector<Trial>trials = c->getTrials();
                    vector<Trial>::iterator t;
                    for(t=trials.begin(); t!=trials.end();t++){
                        r = Record((*s)->getName(),c->getName(),t->getName());
                        foundOrNot = records.find(r);
                        if(!(foundOrNot == notFound)){
                            if(newRecord(t->getResult(),foundOrNot.getRecord(),foundOrNot.getComparisonCriteria(),foundOrNot.getPlace())){
                                //é melhor o recorde registado em toquio - troca
                                r = foundOrNot;
                                records.remove(foundOrNot);
                                r.setDate(t->getDate());
                                r.setRecord(t->getResult());
                                r.setRecordist(t->getWinner());
                                vector<string>::const_iterator it = find(t->getParticipants().begin(), t->getParticipants().end(), t->getWinner());
                                int indexParticipantCountry = distance(t->getParticipants().begin(), it);
                                r.setCountry(t->getCountries()[indexParticipantCountry]);
                                r.setPlace("Tokyo");
                                records.insert(r);
                            }
                        }
                    }
                }
                else{
                    if(newRecord(c->getResult(),foundOrNot.getRecord(),foundOrNot.getComparisonCriteria(),foundOrNot.getPlace())){
                        //é melhor o recorde registado em toquio - troca
                        r = foundOrNot;
                        records.remove(foundOrNot);
                        r.setDate(c->getEnd());
                        r.setRecord(c->getResult());
                        vector<Medal>medals = c->getMedals();
                        if(medals.size() >= 1){
                            r.setRecordist(medals[0].getWinner());
                            r.setCountry(medals[0].getCountry());
                        }
                        else{
                            r.setRecordist("");
                            r.setCountry("");
                        }
                        r.setPlace("Tokyo");
                        records.insert(r);
                    }
                }
            }
        }
    }
}

void Delegation::showAllRecords() const{

    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(22) << " "<< "All-Time Olympic Records" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;


    BSTItrIn<Record> bstit(records);
    while(!bstit.isAtEnd()){
        if(bstit.retrieve().getRecord() != -1){
            bstit.retrieve().showInfo();
            cout << endl;
        }
        bstit.advance();
    }
    if(records.isEmpty()) throw NoRecords();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showRecordsBySport() const{

    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(18) << " "<< "All-Time Olympic Records by Sport" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;

    bool found = false,failed=true;
    string sp;

    do{
        cout << "Choose a sport: ";
        getline(cin,sp);
        if (cin.eof()){
            cin.clear();
            return;
        }
        else if(cin.fail()){
            cin.clear();
        }
        else if(checkStringInput(sp) == 0)
            failed =false;
        if(failed) cout << "Not a sport, please try again!\n";
    }while(failed);

    BSTItrIn<Record> bstit(records);
    while(!bstit.isAtEnd()){
        if(bstit.retrieve().getSport() == sp){
            if(!found) cout << endl;
            found = true;
            bstit.retrieve().showInfo();
            cout << endl;
        }
        bstit.advance();
    }
    if(records.isEmpty()||!found) throw NoRecords();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showRecordsByCompetition() const{
    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(15) << " "<< "All-Time Olympic Records by Competition" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;

    bool found = false,failed;
    string c;

    do{
        cout << "Choose a competition/trial: ";
        getline(cin,c);
        if (cin.eof()){
            cin.clear();
            return;
        }
        else if(cin.fail()){
            cin.clear();
            failed = true;
        } else failed = false;
        if(failed) cout << "Not a possible competition/trial, please try again!\n";
    }while(failed);

    BSTItrIn<Record> bstit(records);
    while(!bstit.isAtEnd()){
        if(bstit.retrieve().getCompetition() == c || bstit.retrieve().getTrial() == c){
            if(bstit.retrieve().getRecord() != -1){
                if(!found) cout << endl;
                found = true;
                bstit.retrieve().showInfo();
                cout << endl;
            }
        }
        bstit.advance();
    }
    if(records.isEmpty()||!found) throw NoRecords();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showTokyoRecords() const{
    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(22) << " "<< "Tokyo 2020 Olympic Records" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;

    bool found = false;

    BSTItrIn<Record> bstit(records);
    while(!bstit.isAtEnd()){
        if(bstit.retrieve().getPlace() == "Tokyo" && bstit.retrieve().getDate().getYear() == 2020){
            found = true;
            bstit.retrieve().showInfo();
            cout << endl;
        }
        bstit.advance();
    }
    if(records.isEmpty() || !found) throw NoRecords();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::showTokyoResults() const {
    vector<Sport *>::const_iterator s;
    Record notFound;
    string sport;

    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(22) << " "<< "Tokyo 2020 Olympic Results" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;

    for (s = sports.begin(); s != sports.end(); s++) {//para cada desporto
        if (!(*s)->isTeamSport()) { //se    for individual vai ter resultados;
            sport = (*s)->getName();
            transform(sport.begin(), sport.end(), sport.begin(), ::toupper);
            for (int j = 0; j < sport.size() * 3; j++)cout << "-";
            cout << endl;
            for (int j = 0; j < sport.size(); j++)cout << " ";
            cout << sport << endl;
            for (int j = 0; j < sport.size() * 3; j++)cout << "-";
            cout << endl;
            vector<Competition> competitions = (*s)->getCompetitions();
            vector<Competition>::iterator c;
            for (c = competitions.begin(); c != competitions.end(); c++) { //ou nas competições
                if (c->getResult() != -2 && c->getResult() != -1) { //os resultados estão nas competições
                    c->showResult();
                    cout << endl;
                } else {//os resultados estão nos jogos
                    vector<Trial> trials = c->getTrials();
                    vector<Trial>::iterator t;
                    for (t = trials.begin(); t != trials.end(); t++) {//ou nos jogos
                        if(t->getResult() != -1){
                            t->showResult();
                            cout << endl;
                        }
                    }
                }
            }
        }//se for desporto de equipa não temos resultados
    }

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::changeTokyoResult(){
    vector<Sport *>::const_iterator s;
    Record notFound;
    string sport, compTrial;

    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(16) << " "<< "Change the Result of a Competition/Game" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;

    bool firstSportComp;
    vector<string> changeResults;

    cout << "Competitions and Trials you can change:\n";
    //show the competitions/games without result
    for (s = sports.begin(); s != sports.end(); s++) {//para cada desporto
        if (!(*s)->isTeamSport()) { //se for individual vai ter resultados;
            sport = (*s)->getName();
            transform(sport.begin(), sport.end(), sport.begin(), ::toupper);
            vector<Competition> competitions = (*s)->getCompetitions();
            vector<Competition>::iterator c;
            firstSportComp = true;
            for (c = competitions.begin(); c != competitions.end(); c++) { //ou nas competições
                if (c->getResult() != -2 && c->getResult() != -1) {
                    if(firstSportComp){
                        cout << sport << endl;
                        firstSportComp = false;
                    }
                    cout << "->" << c->getName() << endl;
                    changeResults.push_back(c->getName());
                } else {//os resultados estão nos jogos
                    vector<Trial> trials = c->getTrials();
                    vector<Trial>::iterator t;
                    for (t = trials.begin(); t != trials.end(); t++) {//ou nos jogos
                        if (t->getResult() != -1) {
                            if(firstSportComp){
                                cout << sport << endl;
                                firstSportComp = false;
                            }
                            cout << "->" <<  t->getName() << endl;
                            changeResults.push_back(t->getName());
                        }
                    }
                    cout << endl;
                }
            }
            cout << endl;
        }//se for desporto de equipa não temos resultados
    }

    //Chose competition/trial to add result to
    bool failed;
    do{
        cout << "Choose a competition/trial: ";
        getline(cin,compTrial);
        if (cin.eof()){
            cin.clear();
            return;
        }
        else if(cin.fail()){
            cin.clear();
            failed = true;
        }
        else if(find(changeResults.begin(),changeResults.end(),compTrial) == changeResults.end()) failed = true;
        else failed = false;
        if(failed) cout << "Not a valid trial/competition, please try again!\n";
    }while(failed);

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(16) << " "<< "Change the Result of a Competition/Game" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;

    //print competition/trial name
    string tmp = compTrial;
    transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    for (int j = 0; j < tmp.size() * 3; j++)cout << "-";
    cout << endl;
    for (int j = 0; j < tmp.size(); j++)cout << " ";
    cout << tmp << endl;
    for (int j = 0; j < tmp.size() * 3; j++)cout << "-";
    cout << endl;

    //check if it is a competition or if it is trial and get possible participants
    bool isComp = true;
    Competition competition;
    Trial trial;
    competition.setName(compTrial);
    trial.setName(compTrial);
    vector<string> participants;
    vector<Competition>::iterator itC;
    vector<Trial>::iterator itT;

    for(s = sports.begin(); s != sports.end(); s++){
        if(!(*s)->isTeamSport()){
            vector<Competition> competitions = (*s)->getCompetitions();
            competition.setName(compTrial);
            itC = find(competitions.begin(),competitions.end(),competition);
            if(!(itC == competitions.end())){
                participants = itC->getParticipants();
                competition = *itC;
                isComp = true;
                break;
            }
            else{
                isComp = false;
                for(itC =competitions.begin(); itC != competitions.end(); itC++){
                    competition = *itC;
                    vector<Trial> trials = itC->getTrials();
                    itT = find(trials.begin(),trials.end(),trial);
                    if(!(itT == trials.end())){
                        trial = *itT;
                        participants = itT->getParticipants();
                        break;
                    }
                }
            }
        }
    }

    //Ask for result and for medals(if a competition) or winner(if a trial)

    //Ask for the new result to set
    float result;
    failed = false;
    do{
        cout << "Result: ";
        getline(cin,tmp);
        if (cin.eof()){
            cin.clear();
            return;
        }
        if (checkFloatInput(tmp) != 0)
            failed = true;
        else{
            if (stof(tmp) < 0)
                failed = true;
            else failed = false;
        }
        if(failed) cout << "Not a valid result, please try again.\n";
    }while(failed);

    result = stof(tmp);

    string participant="",country="";

    vector<string>::iterator participantToErase;
    if(isComp){
        cout << "(Possible Portuguese Participants: ";
        for(unsigned int i = 0; i< participants.size(); i++){
            cout << participants[i];
            if(i < participants.size()-1) cout << ", ";
        }
        cout << ")\n";

        vector<Medal> medals;
        Medal m;

        cout << "\nGold medal winner: \n";
        do{
            cout << "->Winner's country: ";
            getline(cin,country);
            if (cin.eof()){
                cin.clear();
                return;
            }
            if (checkStringInput(country) != 0)
                cout << "Not a valid  country, please try again.\n";
            else failed = false;
        }while(checkStringInput(country) != 0);

        failed = false;
        do{
            cout << "->Winner's name: ";
            getline(cin,participant);
            if (cin.eof()){
                cin.clear();
                return;
            }
            if (checkStringInput(participant) != 0)
                failed = true;
            else if(country == "Portugal"){
                participantToErase = find(participants.begin(), participants.end(),participant);
                if (participantToErase == participants.end())
                    failed = true;
                else failed = false;
            }
            else failed = false;
            if(failed) cout << "Not a valid  participant, please try again.\n";
        }while(failed);
        if(country == "Portugal")
            participants.erase(participantToErase);

        m.setWinner(participant);
        m.setCountry(country);
        m.setType('g');

        medals.push_back(m);

        failed = false;
        cout << "\nSilver medal winner: \n";
        do{
            cout << "->Winner's country: ";
            getline(cin,country);
            if (cin.eof()){
                cin.clear();
                return;
            }
            if (checkStringInput(country) != 0)
                failed = true;
            else if(country == "Portugal" && participants.empty())
                failed = true;
            else failed = false;
            if(failed) cout << "Not a valid  country, please try again.\n";
        }while(failed);

        failed = false;
        do{
            cout << "->Winner's name: ";
            getline(cin,participant);
            if (cin.eof()){
                cin.clear();
                return;
            }
            if (checkStringInput(participant) != 0)
                failed = true;
            else if(country == "Portugal"){
                participantToErase = find(participants.begin(), participants.end(),participant);
                if (participantToErase == participants.end())
                    failed = true;
                else failed = false;
            }
            if(failed) cout << "Not a valid  participant, please try again.\n";
        }while(failed);
        if(country == "Portugal") participants.erase(participantToErase);

        m.setWinner(participant);
        m.setCountry(country);
        m.setType('s');

        medals.push_back(m);

        cout << "\nBronze medal winner: \n";
        failed = false;
        do{
            cout << "->Winner's country: ";
            getline(cin,country);
            if (cin.eof()){
                cin.clear();
                return;
            }
            if (checkStringInput(country) != 0)
                failed = true;
            else if(country == "Portugal" && participants.empty())
                failed = true;
            else failed = false;
            if(failed) cout << "Not a valid  country, please try again.\n";
        }while(failed);

        failed = false;
        do{
            cout << "->Winner's name: ";
            getline(cin,participant);
            if (cin.eof()){
                cin.clear();
                return;
            }
            if (checkStringInput(participant) != 0)
                failed = true;
            else if(country == "Portugal"){
                participantToErase = find(participants.begin(), participants.end(),participant);
                if (participantToErase == participants.end())
                    failed = true;
                else failed = false;
            }
            if(failed) cout << "Not a valid  participant, please try again.\n";
        }while(failed);
        if(country == "Portugal") participants.erase(participantToErase);

        m.setWinner(participant);
        m.setCountry(country);
        m.setType('b');

        medals.push_back(m);

        competition.setResult(result);
        competition.setMedals(medals);

        for (s = sports.begin(); s != sports.end(); s++) {
            if (!(*s)->isTeamSport()) {
                vector<Competition> competitions = (*s)->getCompetitions();
                for (itC = competitions.begin(); itC != competitions.end(); itC++) {
                    if(competition == *itC){
                        competitions.erase(itC);
                        competitions.push_back(competition);
                        (*s)->setCompetitions(competitions);
                        break;
                    }
                }
            }
        }

    }
    else {
        cout << "(Possible Participants: ";
        for (unsigned int i = 0; i < participants.size(); i++) {
            cout << participants[i];
            if (i < participants.size() - 1) cout << ", ";
        }
        cout << ")\n";

        do {
            cout << "Winner: ";
            getline(cin, participant);
            if (cin.eof()) {
                cin.clear();
                return;
            }
            participantToErase = find(participants.begin(), participants.end(), participant);
            if (checkStringInput(participant) != 0 || participantToErase == participants.end())
                cout << "Not a valid  participant, please try again.\n";
        } while (checkStringInput(participant) != 0 || participantToErase == participants.end());

        trial.setResult(result);
        trial.setWinner(participant);

        for (s = sports.begin(); s != sports.end(); s++) {
            if (!(*s)->isTeamSport()) {
                vector<Competition> competitions = (*s)->getCompetitions();
                for (itC = competitions.begin(); itC != competitions.end(); itC++) {
                    competition = *itC;
                    vector<Trial> trials = itC->getTrials();
                    itT = find(trials.begin(), trials.end(), trial);
                    if (!(itT == trials.end())) {
                        trials.erase(itT);
                        trials.push_back(trial);

                        competitions.erase(itC);
                        competition.setTrials(trials);
                        competitions.push_back(competition);

                        (*s)->setCompetitions(competitions);
                        break;
                    }
                }
            }
        }
    }

    setRecords();

    cout << endl << "0 - BACK" << endl;
    do {
        test = checkinputchoice(input, 0, 0);
        if (test != 0&& test != 2)
            cerr << "Invalid option! Press 0 to go back." << endl;
    } while (test != 0 && test != 2);
}

void Delegation::addTokyoResult(){
    vector<Sport *>::const_iterator s;
    Record notFound;
    string sport, compTrial;

    int test = 0;
    string input = "";

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(17) << " "<< "Add the Result of a Competition/Game" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;

    bool firstSportComp = true;
    vector<string> noResults;

    cout << "Competitions and Trials without result:\n";
    //show the competitions/games without result
    for (s = sports.begin(); s != sports.end(); s++) {//para cada desporto
        if (!(*s)->isTeamSport()) { //se    for individual vai ter resultados;
            sport = (*s)->getName();
            transform(sport.begin(), sport.end(), sport.begin(), ::toupper);
            vector<Competition> competitions = (*s)->getCompetitions();
            vector<Competition>::iterator c;
            firstSportComp = true;
            for (c = competitions.begin(); c != competitions.end(); c++) { //ou nas competições
                if (c->getResult() == -1) { //os resultados estão nas competições
                    if(firstSportComp){
                        cout << sport << endl;
                        firstSportComp = false;
                    }
                    cout << "->" << c->getName() << endl;
                    noResults.push_back(c->getName());
                    cout << endl;
                } else {//os resultados estão nos jogos
                    vector<Trial> trials = c->getTrials();
                    vector<Trial>::iterator t;
                    for (t = trials.begin(); t != trials.end(); t++) {//ou nos jogos
                        if (t->getResult() == -1) {
                            if(firstSportComp){
                                cout << sport << endl;
                                firstSportComp = false;
                            }
                            cout << "->" <<  t->getName() << endl;
                            noResults.push_back(t->getName());
                            cout << endl;
                        }
                    }
                }
            }
        }//se for desporto de equipa não temos resultados
    }

    if(noResults.empty()) throw NoMissingResults();

    //Chose competition/trial to add result to
    bool failed;
    do{
        cout << "Choose a competition/trial: ";
        getline(cin,compTrial);
        if (cin.eof()){
            cin.clear();
            return;
        }
        else if(cin.fail()){
            cin.clear();
            failed = true;
        } else failed = false;

        if(find(noResults.begin(),noResults.end(),compTrial) == noResults.end()) failed = true;
        else failed = false;

        if(failed) cout << "Not a valid trial/competition, please try again!\n";
    }while(failed);

    system("cls");
    cout << "----------------------------------------------------------------------" << endl;
    cout << setw(17) << " "<< "Add the Result of a Competition/Game" <<"" << endl;
    cout << "----------------------------------------------------------------------"  << endl << endl;

    //print competition/trial name
    string tmp = compTrial;
    transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    for (int j = 0; j < tmp.size() * 3; j++)cout << "-";
    cout << endl;
    for (int j = 0; j < tmp.size(); j++)cout << " ";
    cout << tmp << endl;
    for (int j = 0; j < tmp.size() * 3; j++)cout << "-";
    cout << endl;

    //check if it is a competition or if it is trial and get possible participants
    bool isComp = true;
    Competition competition;
    Trial trial;
    competition.setName(compTrial);
    trial.setName(compTrial);
    vector<string> participants;
    vector<Competition>::iterator itC;
    vector<Trial>::iterator itT;

    for(s = sports.begin(); s != sports.end(); s++){
        if(!(*s)->isTeamSport()){
            vector<Competition> competitions = (*s)->getCompetitions();
            competition.setName(compTrial);
            itC = find(competitions.begin(),competitions.end(),competition);
            if(!(itC == competitions.end())){
                participants = itC->getParticipants();
                competition = *itC;
                isComp = true;
                break;
            }
            else{
                isComp = false;
                for(itC =competitions.begin(); itC != competitions.end(); itC++){
                    competition = *itC;
                    vector<Trial> trials = itC->getTrials();
                    itT = find(trials.begin(),trials.end(),trial);
                    if(!(itT == trials.end())){
                        trial = *itT;
                        participants = itT->getParticipants();
                        break;
                    }
                }
            }
        }
    }

       //Ask for result and for medals(if a competition) or winner(if a trial)

       //Ask for the new result to set
       float result;
       failed = false;
       do{
           cout << "Result: ";
           getline(cin,tmp);
           if (cin.eof()){
               cin.clear();
               return;
           }
           if (checkFloatInput(tmp) != 0)
               failed = true;
           else{
               if (stof(tmp) < 0)
                   failed = true;
               else failed = false;
           }
           if(failed) cout << "Not a valid result, please try again.\n";
       }while(failed);

       result = stof(tmp);

       string participant="",country="";

       vector<string>::iterator participantToErase;
       if(isComp){
           cout << "(Possible Portuguese Participants: ";
           for(unsigned int i = 0; i< participants.size(); i++){
               cout << participants[i];
               if(i < participants.size()-1) cout << ", ";
           }
           cout << ")\n";

           vector<Medal> medals;
           Medal m;

           cout << "\nGold medal winner: \n";
           do{
               cout << "->Winner's country: ";
               getline(cin,country);
               if (cin.eof()){
                   cin.clear();
                   return;
               }
               if (checkStringInput(country) != 0)
                   cout << "Not a valid  country, please try again.\n";
               else failed = false;
           }while(checkStringInput(country) != 0);

           failed = false;
           do{
               cout << "->Winner's name: ";
               getline(cin,participant);
               if (cin.eof()){
                   cin.clear();
                   return;
               }
               if (checkStringInput(participant) != 0)
                   failed = true;
               else if(country == "Portugal"){
                   participantToErase = find(participants.begin(), participants.end(),participant);
                   if (participantToErase == participants.end())
                       failed = true;
                   else failed = false;
               }
               else failed = false;
               if(failed) cout << "Not a valid  participant, please try again.\n";
           }while(failed);
           if(country == "Portugal")
               participants.erase(participantToErase);

           m.setWinner(participant);
           m.setCountry(country);
           m.setType('g');

           medals.push_back(m);

           failed = false;
           cout << "\nSilver medal winner: \n";
           do{
               cout << "->Winner's country: ";
               getline(cin,country);
               if (cin.eof()){
                   cin.clear();
                   return;
               }
               if (checkStringInput(country) != 0)
                   failed = true;
               else if(country == "Portugal" && participants.empty())
                   failed = true;
               else failed = false;
               if(failed) cout << "Not a valid  country, please try again.\n";
           }while(failed);

           failed = false;
           do{
               cout << "->Winner's name: ";
               getline(cin,participant);
               if (cin.eof()){
                   cin.clear();
                   return;
               }
               if (checkStringInput(participant) != 0)
                   failed = true;
               else if(country == "Portugal"){
                   participantToErase = find(participants.begin(), participants.end(),participant);
                   if (participantToErase == participants.end())
                       failed = true;
                   else failed = false;
               }
               if(failed) cout << "Not a valid  participant, please try again.\n";
           }while(failed);
           if(country == "Portugal") participants.erase(participantToErase);

           m.setWinner(participant);
           m.setCountry(country);
           m.setType('s');

           medals.push_back(m);

           cout << "\nBronze medal winner: \n";
           failed = false;
           do{
               cout << "->Winner's country: ";
               getline(cin,country);
               if (cin.eof()){
                   cin.clear();
                   return;
               }
               if (checkStringInput(country) != 0)
                   failed = true;
               else if(country == "Portugal" && participants.empty())
                   failed = true;
               else failed = false;
               if(failed) cout << "Not a valid  country, please try again.\n";
           }while(failed);

           failed = false;
           do{
               cout << "->Winner's name: ";
               getline(cin,participant);
               if (cin.eof()){
                   cin.clear();
                   return;
               }
               if (checkStringInput(participant) != 0)
                   failed = true;
               else if(country == "Portugal"){
                   participantToErase = find(participants.begin(), participants.end(),participant);
                   if (participantToErase == participants.end())
                       failed = true;
                   else failed = false;
               }
               if(failed) cout << "Not a valid  participant, please try again.\n";
           }while(failed);
           if(country == "Portugal") participants.erase(participantToErase);

           m.setWinner(participant);
           m.setCountry(country);
           m.setType('b');

           medals.push_back(m);

           competition.setResult(result);
           competition.setMedals(medals);

           for (s = sports.begin(); s != sports.end(); s++) {
               if (!(*s)->isTeamSport()) {
                   vector<Competition> competitions = (*s)->getCompetitions();
                   for (itC = competitions.begin(); itC != competitions.end(); itC++) {
                       if(competition == *itC){
                           competitions.erase(itC);
                           competitions.push_back(competition);
                           (*s)->setCompetitions(competitions);
                           break;
                       }
                   }
               }
           }
       }
       else {
           cout << "(Possible Participants: ";
           for (unsigned int i = 0; i < participants.size(); i++) {
               cout << participants[i];
               if (i < participants.size() - 1) cout << ", ";
           }
           cout << ")\n";

           do {
               cout << "Winner: ";
               getline(cin, participant);
               if (cin.eof()) {
                   cin.clear();
                   return;
               }
               participantToErase = find(participants.begin(), participants.end(), participant);
               if (checkStringInput(participant) != 0 || participantToErase == participants.end())
                   cout << "Not a valid  participant, please try again.\n";
           } while (checkStringInput(participant) != 0 || participantToErase == participants.end());

           trial.setResult(result);
           trial.setWinner(participant);

           for (s = sports.begin(); s != sports.end(); s++) {
               if (!(*s)->isTeamSport()) {
                   vector<Competition> competitions = (*s)->getCompetitions();
                   for (itC = competitions.begin(); itC != competitions.end(); itC++) {
                       competition = *itC;
                       vector<Trial> trials = itC->getTrials();
                       itT = find(trials.begin(), trials.end(), trial);
                       if (itT != trials.end()) {
                           trials.erase(itT);
                           trials.push_back(trial);

                           competitions.erase(itC);
                           competition.setTrials(trials);
                           competitions.push_back(competition);

                           (*s)->setCompetitions(competitions);
                           break;
                       }
                   }
               }
           }
       }

       setRecords();

       cout << endl << "0 - BACK" << endl;
       do {
           test = checkinputchoice(input, 0, 0);
           if (test != 0&& test != 2)
               cerr << "Invalid option! Press 0 to go back." << endl;
       } while (test != 0 && test != 2);
}


//File Errors - Exceptions
FileError::FileError(string file) : file(move(file)) {}

ostream &operator<<(ostream &os, const FileError &file) {
    os << "Error opening file " << file.file << "!" << endl;
    return os;
}

FileStructureError::FileStructureError(string file) : file(move(file)) {}

ostream &operator<<(ostream &os, const FileStructureError &file) {
    os << "The structure of file " << file.file << " is not the expected!" << endl;
    return os;
}

//sport doesn't exist
NonExistentSport::NonExistentSport(string name) {
    this->sport = name;
}

ostream &operator<<(ostream &os, const NonExistentSport &c) {
    os << "The Delegation does not take part in " << c.sport << "!\n";
    return os;
}

//competition doesn't exist
NonExistentCompetition::NonExistentCompetition(string name, string sport) {
    this->competition = name;
    this->sport = sport;
}

ostream &operator<<(ostream &os, const NonExistentCompetition &c) {
    os << c.competition << " doesn't exist in " << c.sport << "!\n";
    return os;
}

//trial doesn't exist
NonExistentTrial::NonExistentTrial(string name, string competition, string sport) {
    this->name = name;
    this->competition = competition;
    this->sport = sport;
}

ostream &operator<<(ostream &os, NonExistentTrial &t) {
    os << t.name << " doesn't exist in " << t.competition << ", " << t.sport << "!\n";
    return os;
}

NonExistentPerson::NonExistentPerson(string name) {
    person = name;
}

ostream &operator<<(ostream &os, NonExistentPerson &p) {
    os << p.person << " doesn't exist!\n";
    return os;
}

NonExistentAthlete::NonExistentAthlete(string name) {
    person = name;
}

ostream &operator<<(ostream &os, NonExistentAthlete &p) {
    os << p.person << " is not an athlete!\n";
    return os;
}

NonExistentStaff::NonExistentStaff(string name) {
    person = name;
}

ostream &operator<<(ostream &os, NonExistentStaff &p) {
    os << p.person << " is not a member of the staff!\n";
    return os;
}
NonExistentTeam::NonExistentTeam(string name) {
    team = name;
}

ostream &operator<<(ostream &os, NonExistentTeam &p) {
    os << p.team << " doesn't exist!\n";
    return os;
}

PersonAlreadyExists::PersonAlreadyExists(string person) {
    this->person = person;
}

ostream &operator<<(ostream &os, PersonAlreadyExists &p) {
    os << p.person << " already exists!\n";
    return os;
}

TeamAlreadyExists::TeamAlreadyExists(string team) {
    this->team = team;
}

ostream &operator<<(ostream &os, TeamAlreadyExists &p) {
    os << p.team << " already exists!\n";
    return os;
}

NoMembers::NoMembers() {}

ostream &operator<<(ostream &os, NoMembers &p) {
    os << " No members to show!\n";
    return os;
}

NoSports::NoSports(){}

ostream & operator <<(ostream & os, const NoSports & s) {
    os << " No sports to show!\n";
    return os;
}

NoCompetitions::NoCompetitions(const string & sport){this->sport = sport;}

ostream &operator<<(ostream &os, NoCompetitions &p) {
    os << " No " << p.sport << " competitions to show!\n";
    return os;
}

NoTrials::NoTrials(const string & sport){this->sport = sport;}

ostream &operator<<(ostream &os, NoTrials &p) {
    if(p.sport!="")
        os << p.sport << " competitions don't have any trials!\n";
    else
        os << "The Sports of the Delegation don't have Trials!"<<endl;
    return os;
}

NoMedals::NoMedals() {country="";}

NoMedals::NoMedals(const string & c){country=c;}

ostream &operator<<(ostream &os, NoMedals &m) {
    if(m.country=="")
        os << " No medals to show!\n";
    else
        os << m.country << " didn't win any medals!\n";
    return os;
}

NotATeamSport::NotATeamSport(const string &s){sport=s;}

ostream &operator<<(ostream &os, NotATeamSport &p) {
    os << p.sport << " is not a team sport!\n";
    return os;
}

FullTeam::FullTeam(const string &t) {team = t;}

ostream &operator<<(ostream &os, FullTeam &t) {
    os << t.team << " is full!\n";
    return os;
}

NoRecords::NoRecords(){}

ostream & operator <<(ostream & os, const NoRecords & r) {
    os << "No records to show!\n";
    return os;
}

NoMissingResults::NoMissingResults(){}

ostream & operator <<(ostream & os, const NoMissingResults & r) {
    os << "No results missing!\n";
    return os;
}