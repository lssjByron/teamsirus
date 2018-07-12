#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <sstream>

#include "../database/CSVReader.h"


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


    //store the header check vectors onto a vector and get the headers of the csvreader object
    std::vector<std::vector<std::string>> headerCheck = {teachHeaderCheck,publicationsHeaderCheck,presentationsHeaderCheck,fundingHeaderCheck,teachExpandedHeaderCheck,publicationsExpandedHeaderCheck,presentationsExpandedHeaderCheck,grantsExpandedHeaderCheck};
    std::vector<std::string> header = this->getHeaders();
    // To check if the input file is valid:
    //simply check for equality between the input header and the header to the corresponding CSV type, and return the type
    //associated with the file type
    if(header == headerCheck[0] || header == headerCheck[4])//Teach
        return 0;
    else if(header == headerCheck[1] || header == headerCheck[5])//Publications
        return 1;
    else if(header == headerCheck[2] || header == headerCheck[6])//presentations
        return 2;
    else if(header == headerCheck[3] || header == headerCheck[7])//fund
        return 3;

    // otherwise return -1
    return -1;

}

// Loads the CSV file at file_name.

void CSVReader::loadCSV(std::string file_name) {
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
                }
                line.push_back( element );

                if ( f.at( pos ) == '\n') {
                    break;
                }
                pos++;
            }
            if( !setHeaders ) {
                setHeaders = true;
                headers = line;
            } else {
                //checks if element in line is just whitespace, if it is change it to an empty string
                all_data.push_back( line );
            }
            pos++;
        }
    }
}

void CSVReader::loadCSVFixed(std::string file_name) {
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
                            if(pos == len -1)
                                break;
                            element += f.at( pos );
                            pos++;
                        }
                        pos++; // Last quote
                    } else {
                        element += f.at( pos );
                        pos++;
                    }
                    if(pos == len - 1)
                        break;
                }
                line.push_back( element );

                if ( f.at( pos ) == '\n' || pos == len - 1) {
                    break;
                }
                pos++;
            }
            if( !setHeaders ) {
                setHeaders = true;
                headers = line;
            } else {
                for(size_t i = 0; i < line.size(); i++){
                    if(line[i].find_first_not_of (' ') == line[i].npos)
                        line[i] = "";
                }
                if(!std::equal(line.begin() + 1, line.end(), line.begin())){
                    all_data.push_back( line );
                }
            }
            pos++;
        }
    }
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

std::vector<std::vector<std::string> > CSVReader::getData() {
    return all_data;
}

CSVReader::CSVReader() {

}

CSVReader::CSVReader(std::string file) {
    loadCSV(file);
}

CSVReader::CSVReader(std::string file, bool is_fixed) {
    if(is_fixed)
        loadCSVFixed(file);
    else
        loadCSV(file);
}
