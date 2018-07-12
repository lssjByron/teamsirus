#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <qdebug.h>


#include "database/CSVReader.h"
//#include "database/RecordsManager.h"
std::vector<std::string> CSVReader::GRANTS_MANFIELDS = {"Member Name", "Funding Type", "Status", "Peer Reviewed?", "Role", "Title", "Start Date"};
std::vector<std::string> CSVReader::PRES_MANFIELDS = {"Member Name", "Date", "Type", "Role", "Title"};
std::vector<std::string> CSVReader::PUBS_MANFIELDS = {"Member Name", "Type", "Status Date", "Role", "Title"};
std::vector<std::string> CSVReader::TEACH_MANFIELDS = {"Member Name", "Start Date", "Program"};
// Loads the CSV file at file_name.

void CSVReader::loadCSV(std::string file_name) {
    path = file_name;
    std::ifstream myfile(file_name.c_str());
    if (myfile.fail()) {
        std::cout << "Couldn't open file \"" << file_name << "\"." << std::endl;
        return;
    }
	
    if (myfile.is_open()) {
		std::stringstream sstr;
		sstr << myfile.rdbuf();
		std::string f = sstr.str();
		myfile.close();

        size_t len = f.length();
		
		bool setHeaders = false;
		
        size_t pos = 0;
        while( pos < len ) { // While the file's not empty
			std::vector<std::string> line;
            while( f.at( pos ) != '\n' && pos < len ) { // For each character in the line
				std::string element = "";
                while( f.at( pos ) != ',' && pos < len && f.at( pos ) != '\n' && f.at( pos ) != '\r' ) { // For each element
					if( f.at( pos ) == '"' ) { // If we have a quote, continue till the next quote
						pos++;
                        while( f[pos] != '"' && pos < len ) {
							element += f.at( pos );
							pos++;
						}
                        pos++; // Last quote
                    } else {
						element += f.at( pos );
                        pos++;
					}
                    if(pos == len -1)
                        break;
				}
                line.push_back( element );

                if ( f.at( pos ) == '\n' || pos == len -1) {
                    break;
                }
                pos++;
			}
			if( !setHeaders ) {
				setHeaders = true;
				headers = line;
			} else {
                //checks if element in line is just whitespace, if it is change it to an empty string
                for(size_t i = 0; i < line.size(); i++)
                    if(line[i].find_first_not_of (' ') == line[i].npos)
                        line[i] = "";
                //if the entire vector is full of empty values, don't add it to vector
                if(!std::equal(line.begin() + 1, line.end(), line.begin()))
                    all_data.push_back( line );
            }
            pos++;
		}
        index_file_type = validHeaders();
        addFileErrors();
        if(index_file_type == 1)//publications
        {
           //createCollaborations();
        }
    }
}

void CSVReader::createCollaborations()
{

    CollabWebGraph graph; // create a new collab web graph to be populated with data derived below:

//    //string is for title name, vector is for authors who worked on title
//    std::vector<std::map<std::string, std::vector<std::string>>> collaborations;
//    std::vector<std::string> titles;

    //RYAN'S IMPLEMENTATION:

    int index_member_name = record_manager->getHeaderIndex("Member Name");
    int index_title = record_manager->getHeaderIndex("Title");

    //COPY MEMBERNAME COLUMN INTO A QLIST<QSTRING>
    QList<QString> memberNameColumn;
    for(int i=0;i< all_data.size();++i)
    {
        memberNameColumn.append(  QString::fromStdString(all_data[i][index_member_name]));
    }


    //DERIVE LIST OF UNIQUE MEMBERNAMES FROM "MemberNameColumn" QList
    QList<QString> uniqueMemberNames;
    if(!memberNameColumn.isEmpty())
    {
        //ADD FIRST MEMBER NAME BECAUSE IT IS UNIQUE
        uniqueMemberNames.append(memberNameColumn[0]);
        for (int i=1;i<memberNameColumn.size();++i)
        {
            if(!uniqueMemberNames.contains(memberNameColumn[i]) && !memberNameColumn[i].isEmpty())
            {
                uniqueMemberNames.append(memberNameColumn[i]);
            }
        }
    }
    //VERIFY DATA:
//        foreach(QString item, uniqueMemberNames)
//        qDebug() << item;

    //qDebug() << "UniqueMembers Size:" << uniqueMemberNames.size();

    //FOR EACH UNIQUE MEMBER --> DERIVE A NEW LIST FOR EACH THAT CONTAINS THE PAPERS THEY EACH WORKED ON (NOTHING TO DO WITH COLLABORATIONS YET)

    QList<QString> memberPapers;
    QList<QList<QString>> uniqueMembersAndPapers;

    for (int i=0; i < uniqueMemberNames.size();++i) // go through unqiue list of members
    {
        memberPapers.clear();

        for(int j=0; j<memberNameColumn.size();++j) // go through member column of all member data
        {
            // uniqueMembers[i] < get the next unique member and compare
            if(QString::compare(uniqueMemberNames[i],memberNameColumn[j])==0) // if unique member name matches the unique member name, use this same index to check the pub title column and add it to this member's list of papers (if it hasn't been added already)
            {
                if(!memberPapers.contains(memberNameColumn[j]) && !QString::fromStdString(all_data[j][index_title]).isEmpty() ) // if it hasn't been added already and if the paper title field is not empty, then add it:
                {
                    memberPapers.append(  QString::fromStdString(all_data[j][index_title])  ); //add the paper
                }
            }
        }
        uniqueMembersAndPapers.append(memberPapers);
    }

    // QList<QList<QString>> uniqueMembersAndPapers just a list of lists of paper titles in the form of QStrings (no member name data here)

//VERIFY DATA:
//    for (int i=0;i<uniqueMembersAndPapers.size();++i)
//    {
//        for(int j=0;j<uniqueMembersAndPapers[i].size();++j)
//        {
//            qDebug() << (uniqueMembersAndPapers[i])[j];
//        }

//    }

//    qDebug() << "testing output at this point";


    // CREATE COLLABWEBGRAPH::COLLABDATA STRUCTS to be read by web graph code

    CollabWebGraph::CollabData memberCollabData;
    //QList<QPair<QString, QString> > collabdNameAndPaper;
    //QList<QString> uniqueMemberPapers;
    bool collaborated = false;
    for (int i=0;i<uniqueMembersAndPapers.size();++i)
    {
        memberCollabData.authName.clear(); memberCollabData.datails.clear(); collaborated=false; // reset all temp variables & 'collaborated' set back to false

        foreach(QString paper, uniqueMembersAndPapers[i])
        {
            for (int j=0; j<all_data.size();++j )
            {
                if(QString::compare(paper,QString::fromStdString(all_data[j][index_title]))==0)
                {
                    if(QString::compare(QString::fromStdString(all_data[j][index_member_name]),uniqueMemberNames[i])!=0)
                    {
                        if(!memberCollabData.datails.contains(QPair<QString,QString>(QString::fromStdString(all_data[j][index_member_name]),paper)))    //"if this specific collaboration has not already been added" (specific as in, the same author AND same paper title has already been entered in)
                            {
                                memberCollabData.authName = uniqueMemberNames[i]; collaborated = true; //if we enter pass this if statement, a collaboration has been successfully detected for this uniqueAuthor. Make sure its relevant data is added (monitored by the "collaborated" bool variable)
                                memberCollabData.datails.append(QPair<QString,QString>(QString::fromStdString(all_data[j][index_member_name]),paper));
                            }
                    }
                }
            }
        }
        if(collaborated) //if true, then append the collab data
         { allCollaboData.append(memberCollabData); }   //"memberCollabData" represents a node on the graph and its relevant data (the author name that he node represents (CollabData.authName) and the list of authors he/she has collab'd with and on what papers (CollabData.details)
    }

    //VERIFY DATA

//    foreach(CollabWebGraph::CollabData collabDataItem, allCollaboData)
//    {
//        qDebug() << "";qDebug() << "";qDebug() << "";
//        qDebug() << collabDataItem.authName;
//        QList<QPair<QString, QString> > deets = collabDataItem.datails;
//        for (int i =0; i< deets.size();++i)
//        {
//            qDebug() << "Author:" << deets[i].first;
//            qDebug() << "Paper:" << deets[i].second;
//        }
//        qDebug() << "";qDebug() << "";qDebug() << "";
//    }


    graph.init(allCollaboData,QString::fromStdString(path));
    graph.exec();

    //std::sort(reOrderedMemberData.begin(),reOrderedMemberData.end());                                                               //SORT THIS DATA SO I CAN USE THE ERASE() FUNCTION WHICH REMOVES DUPLICATES ie. PRODUCES A VECTOR WITH ONLY UNQIUE MEMBER NAMES
    //reOrderedMemberData.erase(std::unique(reOrderedMemberData.begin(),reOrderedMemberData.end()),reOrderedMemberData.end());        //APPLY ERASE FUNCTION to reOrderedMemberData <-- now has a unique list of all members; print this list to verify

//    for (int i=0; i< reOrderedMemberData.size();++i)
//    {
//        std::cout << reOrderedMemberData[i] << '\n';
//    }
    //std::cout << reOrderedMemberData.size() << '\n';


    //CONVERT ALL TITLE DATA to lowercase to ensure a valid comparison
//    for(int i = 0; i < temp_data.size(); ++i){
//         std::for_each(temp_data[i][index_title].begin(), temp_data[i][index_title].end(), tolower);
//    }

//    std::vector<int>::iterator it;
//     it = std::unique (myvector.begin(), myvector.end());   // 10 20 30 20 10 ?  ?  ?  ?
//                                                            //                ^

//     myvector.resize( std::distance(myvector.begin(),it) ); // 10 20 30 20 10

//    std::vector<std::string>::iterator it;
//    it = std::unique(temp_data.begin(),temp_data.end());







    //END OF RYAN'S IMPLEMENTATION

    /*

    for(int i = 0; i < temp_data.size(); i++){
        //check for new title
        if (std::find(titles.begin(), titles.end(), temp_data[i][index_title]) == titles.end()){
            //push back new title
            titles.push_back(temp_data[i][index_title]);
            std::vector<std::string> member_names;
            for(int c = 0; c < temp_data.size(); c++){
                //check if the last element in titles (the newly pushed title) equals the current value in temp_data, and if the members name can't be found in member_names
                if(temp_data[c][index_title] == titles[titles.size()-1] && std::find(member_names.begin(), member_names.end(), temp_data[c][index_member_name]) == member_names.end()){
                    member_names.push_back(temp_data[c][index_member_name]);
                }
            }
            std::map<std::string, std::vector<std::string>> temp_collab;
            temp_collab.insert(std::make_pair(titles[titles.size()-1], member_names));
            collaborations.push_back(temp_collab);
        }
    }

    for(int i = 0; i < collaborations.size(); i++){
        std::vector<std::string> collaboration_authors; //holds authors who collaborated with current author
        //iterate through map (first->string of titles, second->vector of authors)
        for(const auto& collab : collaborations[i] ){
            std::map<std::string, std::map<std::string, std::vector<std::string>>> temp_author_title_collaborators;
            std::map<std::string, std::vector<std::string>> temp_title_and_collaborators;
            std::vector<std::string> temp_authors = collab.second;
            //iterate through authors who collaborated with current author
            for(int counter = 0; counter < temp_authors.size(); counter++){
                std::string current_author = temp_authors[counter];
                collaboration_authors = temp_authors;
                collaboration_authors.erase(collaboration_authors.begin() + counter);
                temp_title_and_collaborators.insert(std::make_pair(collab.first, collaboration_authors));
                temp_author_title_collaborators.insert(std::make_pair(current_author, temp_title_and_collaborators));
            }
            author_collaborations.push_back(temp_author_title_collaborators);
        }
    }
    */


}

void CSVReader::popupCollaborations(){
    CollabWebGraph graph;
    graph.init(allCollaboData,QString::fromStdString(path));
    graph.exec();
}

void CSVReader::addFileErrors(){
    std::string date = "Start Date";
    bool is_grants = false;
    switch(index_file_type){
    case FUNDING:
        mandatory_fields = GRANTS_MANFIELDS;
        is_grants = true;
        break;
    case PRESENTATIONS:
        mandatory_fields = PRES_MANFIELDS;
        date = "Date";
        break;
    case PUBLICATIONS:
        mandatory_fields = PUBS_MANFIELDS;
        date = "Status Date";
        break;
    case TEACH:
        mandatory_fields = TEACH_MANFIELDS;
        break;
    }

    //GRANTS_MANFIELDS, PRES_MANFIELDS, PUBS_MANFIELDS, TEACH_MANFIELDS;
    record_manager = new RecordsManager(&headers);

    // load in data into the manager, with the date as the key
    sortHeaderIndex = record_manager->getHeaderIndex(date);
    unsigned int j;
    for (int i = 0; i < (int) all_data.size(); i++) {
        if(all_data[i].size() < headers.size()){
            while(all_data[i].size() != headers.size()){
                all_data[i].push_back("");
            }
        }

        for (j = 0; j < mandatory_fields.size(); j++) {
            int index = record_manager->getHeaderIndex(mandatory_fields[j]);
            if (all_data[i][index].compare("") == 0) {
                file_errors.push_back(&all_data[i]);
                break;
            }
        }

        // if all mandatory fields are okay
        if (j == mandatory_fields.size()) {
            // date interpretation
            int yrIndex = record_manager->getHeaderIndex(date);
            int year;
            sscanf(all_data[i][yrIndex].c_str(), "%4d", &year);
            all_data[i][yrIndex] = std::to_string(year);
            if(is_grants){
                int prIndex = record_manager->getHeaderIndex("Peer Reviewed?");
                if (all_data[i][prIndex].compare("True") == 0) {
                    all_data[i][prIndex] = "Peer Reviewed";
                } else {
                    all_data[i][prIndex] = "Not Peer Reviewed";
                }
            }
            record_manager->addRecord(parseDateString(all_data[i][sortHeaderIndex]), &all_data[i]);
        }
    }
}

// Checks to see if the headers are valid
int CSVReader::validHeaders(){

    //vectors that store the headers of each file type
    std::vector<std::string> teachHeaderCheck           = {"Record Info", "Last Modified User", "Last Modified Date", "ID", "Member Name", "Primary Domain", "Start Date", "End Date", "Program", "Type of Course / Activity", "Course / Activity", "Geographical Scope", "Institution / Organization", "Faculty", "Department", "Division", "Location", "Hours per Teaching Session or Week", "Number of Teaching Sessions or Weeks", "Faculty Member Additional Comments", "Number Of Trainees", "Student Name(s)", "Initial Lecture", "Faculty Development", "Stipend Received", "Comment", "Other Details (doesn't print)", "Total Hours"};
    std::vector<std::string> publicationsHeaderCheck    = {"Record Info", "Last Modified User", "Last Modified Date", "ID", "Member Name", "Primary Domain", "Publication Status", "Pubmed Article ID", "Type", "Area", "Status Date", "Role", "Peer Reviewed?", "Number Of Contributors", "Author #", "Journal Name | Published In | Book Title | etc.", "Volume", "Issue", "Page Range", "DOI", "Website", "Number of Citations", "Journal Impact Factor", "International", "Publication Country", "Publication Province", "Publication City", "Publisher", "Level of Contribution", "Is Presentation?", "Impact", "Open Access?", "Personal Remuneration", "Is Trainee?", "Trainee Details", "Is Most Significant Publication?", "Most Significant Contribution Details", "Education Publication", "Other Details (doesn't print)", "Author(s)", "Title", "Rest of Citation", "ISBNISSN", "Funding Reference Number"};
    std::vector<std::string> presentationsHeaderCheck   = {"Record Info", "Last Modified User", "Last Modified Date", "ID", "Member Name", "Primary Domain", "Date", "Type", "Area", "Role", "Activity Type", "Geographical Scope", "Host", "Country", "Province", "City", "Number of Attendees", "Main Audience", "Hours", "Teaching Effectiveness Score", "URL", "Competitive", "Education Presentation", "Remarks", "Funding Organization", "Authorship", "Title", "Rest of Citation", "Personal Remuneration", "Other Details (doesn't print)"};
    std::vector<std::string> fundingHeaderCheck         = {"Record Info", "Last Modified User", "Last Modified Date", "ID", "Member Name", "Primary Domain", "Start Date", "End Date", "Funding Type", "Status", "Peer Reviewed?", "Industry Grant?", "Role", "Short Title", "Title", "Application Summary", "Grant Purpose", "Area", "Principal Investigator", "Co-Investigators", "Grant and/or Account #", "Prorated Amount", "Administered By", "Funding Source", "Project", "Currency", "Received Amount", "Total Amount", "Member Share", "Monetary", "Rpt", "Hours Per Week", "Personnel Paid", "Rnw", "Education Grant", "Duplicate Reported", "Other Details (doesn't print)", "Year"};
    // Another set of vectors. These were made since the 'expanded' csv in the /projectInfromation directory had different headers
    std::vector<std::string> teachExpandedHeaderCheck			= {"Record Info","Last Modified User","Last Modified Date","ID","Member Name","Primary Domain","Start Date","End Date","Program","Type of Course / Activity","Course / Activity","Geographical Scope","Institution / Organization","Faculty","Department","Division","Location","Hours per Teaching Session or Week","Number of Teaching Sessions or Weeks","Faculty Member Additional Comments","Number Of Trainees","Student Name(s)","Initial Lecture","Faculty Development","Comment","Other Details (doesn't print)","Total Hours"};
    std::vector<std::string> publicationsExpandedHeaderCheck 	= {"Record Info","Last Modified User","Last Modified Date","ID","Member Name","Primary Domain","Publication Status","Pubmed Article ID","Type","Area","Status Date *","Role *","Peer Reviewed?","Author #","Journal Name | Published In | Book Title | etc.","Volume","Issue","Page Range","DOI","Website","Journal Impact Factor","International","Publisher","Is Presentation?","Personal Remuneration","Trainee Details","Is Most Significant Publication?","Most Significant Contribution Details","Education Publication","Other Details (doesn't print)","Author(s)","Title","Rest of Citation","ISBNISSN"};
    std::vector<std::string> presentationsExpandedHeaderCheck	= {"Record Info","Last Modified User","Last Modified Date","ID","Member Name","Primary Domain","Date","Type","Area","Role","Activity Type","Geographical Scope","Host","Country","Province","City","Number of Attendees","Hours","Teaching Effectiveness Score","Education Presentation","Remarks","Authorship","Title","Rest of Citation","Personal Remuneration"};
    std::vector<std::string> grantsExpandedHeaderCheck			= {"Record Info","Last Modified User","Last Modified Date","ID","Member Name","Primary Domain","Start Date","End Date","Funding Type","Status","Peer Reviewed?","Industry Grant?","Role","Short Title","Title","Application Summary","Grant Purpose","Area","Principal Investigator","Co-Investigators","Grant and/or Account #","Administered By","Funding Source","Project","Currency","Received Amount","Total Amount","Member Share","Monetary","Rpt","Hours Per Week","Personnel Paid","Rnw","Education Grant","Duplicate Reported","Other Details (doesn't print)","Year"};
    // Another set of vectors. These were made for updated 'expanded' csv in the /projectInfromation directory had different headers
    std::vector<std::string> teachExpandedHeaderCheckUpdated			= {"ID", "Member Name", "Department", "Division", "Start Date", "End Date", "Program", "Type of Course / Activity", "Course / Activity", "Geographical Scope", "Institution / Organization", "Faculty", "Department", "Division", "Location", "Hours per Teaching Session or Week", "Number of Teaching Sessions or Weeks", "Number Of Trainees", "Total Hours"};
    std::vector<std::string> publicationsExpandedHeaderCheckUpdated 	= {"ID" ,"Member Name","Department","Division","Publication Status","Pubmed Article ID","Type","Area","Status Date","Role","Peer Reviewed?", "Journal Name | Published In | Book Title | etc.", "Is Presentation?", "Is Most Significant Publication?", "Education Publication","Author(s)","Title"};
    std::vector<std::string> presentationsExpandedHeaderCheckUpdated	= {"ID", "Member Name", "Department", "Division", "Date", "Type", "Role", "Activity Type", "Geographical Scope", "Host", "Country", "Province", "City", "Hours", "Education Presentation", "Authorship", "Title"};
    std::vector<std::string> grantsExpandedHeaderCheckUpdated			= {"ID", "Member Name", "Department", "Division", "Start Date", "End Date", "Funding Type", "Status", "Peer Reviewed?", "Industry Grant?", "Role", "Title", "Grant Purpose", "Area", "Principal Investigator", "Co-Investigators", "Administered By", "Funding Source", "Total Amount"};


    //store the header check vectors onto a vector and get the headers of the csvreader object
    std::vector<std::vector<std::string>> headerCheck = {teachHeaderCheck,publicationsHeaderCheck,presentationsHeaderCheck,fundingHeaderCheck,teachExpandedHeaderCheck,publicationsExpandedHeaderCheck,presentationsExpandedHeaderCheck,grantsExpandedHeaderCheck,teachExpandedHeaderCheckUpdated, publicationsExpandedHeaderCheckUpdated, presentationsExpandedHeaderCheckUpdated, grantsExpandedHeaderCheckUpdated};
    std::vector<std::string> header = this->getHeaders();
    // To check if the input file is valid:
    //simply check for equality between the input header and the header to the corresponding CSV type, and return the type
    //associated with the file type
    if(header == headerCheck[0] || header == headerCheck[4] || header == headerCheck[8])//Teach
        return 0;
    else if(header == headerCheck[1] || header == headerCheck[5] || header == headerCheck[9])//Publications
        return 1;
    else if(header == headerCheck[2] || header == headerCheck[6]|| header == headerCheck[10])//presentations
        return 2;
    else if(header == headerCheck[3] || header == headerCheck[7]|| header == headerCheck[11])//fund
        return 3;

    // otherwise return -1
    return -1;

}

// Returns the year in a date string.

int CSVReader::parseDateString(std::string dateString) {
    int year;
    sscanf(dateString.c_str(), "%4d", &year); // The only 4-digit number in a date field is a year, period
    return year;
}

// Returns the header vector for a CSV

std::vector<std::string> CSVReader::getHeaders() {
    return headers;
}

// Returns all data from a CSV

std::vector<std::vector<std::string>> CSVReader::getData() {
    return all_data;
}

RecordsManager* CSVReader::getRecordManager(){
    return record_manager;
}

std::vector<std::vector<std::string>*> CSVReader::getFileErrors(){
    return file_errors;
}

int CSVReader::getSortHeaderIndex(){
    return sortHeaderIndex;
}

int CSVReader::getIndexFileType(){
    return index_file_type;
}

std::string CSVReader::getPath(){
    return path;
}

std::vector<std::string> CSVReader::getMandatoryFields(){
    return mandatory_fields;
}

QList<CollabWebGraph::CollabData> CSVReader::getCollaborations(){
    return allCollaboData;
}

void CSVReader::setFileErrors(int row, int col, std::string value){
    file_errors[row]->at(col) = value;
}

CSVReader::CSVReader() {

}

CSVReader::CSVReader(std::string file) {
    loadCSV(file);
}


//For the Category Filtering
std::vector<std::vector<std::string>> CSVReader::getUniqueMembersForHeaders(std::vector<std::string> listOfHeaders) {
    std::vector<std::vector<std::string>> tmpVectorOfVectors;
    std::vector<std::string> tmpVectorOfStrings;
    std::string tmpStr;
    std::string tmpStr2;

    int tmpIndex;
    //look for the headers in listOfHeaders and see if they e

    //loop through the columns for a given header in list of headers....
    //for (int i=0; i < listOfHeaders.size(); i++) {
        //for that given header, look through it's cells and build a list of the unique ones
      //  tmpStr2 = listOfHeaders.at(i);
        //tmpIndex = record_manager->getHeaderIndex(tmpStr2);

        //if (tmpIndex == -1) { //header Not Found
            //do something
        //}
        //else {

            for(int c = 0; c < listOfHeaders.size(); c++){
                int index_header = record_manager->getHeaderIndex(listOfHeaders[c]);
                for(int i = 0; i < all_data.size(); i++){
                    //check for new title
                    if (std::find(tmpVectorOfStrings.begin(), tmpVectorOfStrings.end(), all_data[i][index_header]) == tmpVectorOfStrings.end()){
                        //push back new title
                        tmpVectorOfStrings.push_back(all_data[i][index_header]);
                    }
                }
            }

            tmpVectorOfVectors.push_back(tmpVectorOfStrings);
        //}
   // }

    return tmpVectorOfVectors;
}


