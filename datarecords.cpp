/*==========================================================================================
You're tasked with building a simple data analysis system
called DataManager to manage and analyse a collecton of DataRecords. The DataRecord
holds a series of numerical values. The DataManager should be capable of performing
collectve analysis across all samples. Additonally, you are required to implement a custom
excepton class DataExcepton; and DataRecord’s constructor should throw an error when
its data contains negative values.
==========================================================================================*/

#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

// Custom exception class for errors (opening file, negative values and no samples available)
class DataException : public std::exception {
    private:
        string message;
    public:
        DataException (const string &message) : message(message) {}
        const char *what() const throw() {
            return message.c_str();
        }
};

class DataVersion {
    private:
        string version;
    public:
        DataVersion(const string &version = "v1.0.0") : version(version) {}
        friend ostream& operator<<(ostream& out, const DataVersion& other) {
            out << other.version;
            return out;
        }
        virtual void print_info() const = 0;
};

class DataRecord : public DataVersion {
    public:
        vector<double> dataVector;

        // Constructor
        DataRecord(const string& version, const vector<double>& dataValues) : DataVersion(version), dataVector(dataValues) {
            try {
                if (any_of(dataVector.begin(), dataVector.end(), [](double val) { return val < 0; })) {
                    throw DataException("Error: Negative values are not allowed in the data sample.");
                }
            } catch (const exception &e) {
                cout << e.what() << endl;
            }
        }

        // Function to calculate mean of sample
        double getStatisticsA() const {
            double sum = accumulate(dataVector.begin(), dataVector.end(), 0.0);
            return sum / dataVector.size(); 
        }

        // Function to calculate standard deviation of sample
        double getStatisticsB() const {
            double mean = getStatisticsA();
            double sq_sum = inner_product(dataVector.begin(), dataVector.end(), dataVector.begin(), 0.0);
            return sqrt(sq_sum / dataVector.size() - mean * mean);
        }

        void print_info() const override {}
};

class DataManager : public DataVersion {
    private:
        vector<DataRecord> samples;

        // Load samples from file and add them to samples vector
        void load_samples(const string& version, const string& filename) {
            ifstream new_file(filename);
            if (!new_file.is_open()) {
                throw DataException("Cannot open file: " + filename);
            }
            string line;
            while (getline(new_file, line)) {
                istringstream iss(line);
                vector<double> dataValues;
                double num;
                while (iss >> num) {
                    dataValues.push_back(num);
                }
                samples.emplace_back(version, dataValues);
            }
            new_file.close();
        }

    public:
        void operator()(const string& version, const string& filename) {
            load_samples(version, filename); // Load new samples
        }

        // Constructor
        DataManager(const string& version = "v1.0.0", const string& filename = "") : DataVersion(version) {}

        // Destructor
        ~DataManager() {
            samples.clear(); // Clear existing samples
        }

        // Count samples where the maximum value exceeds triple its mean value
        int countTripleMeanSamples() const {
            int count = 0;
            for (const auto& record : samples) {
                double mean = record.getStatisticsA();
                double max_value = *max_element(record.dataVector.begin(), record.dataVector.end());
                if (max_value > 3 * mean) {
                    count++;
                }
            }
            return count;
        }

        // Get the sample with the highest mean value
        DataRecord getBestSample() const {
            if (samples.empty()) {
                throw DataException("No samples available.");
            }
            auto best = max_element(samples.begin(), samples.end(), [](const DataRecord& a, const DataRecord& b) {
                return a.getStatisticsA() < b.getStatisticsA();
            });
            return *best;
        }

        // Output all relevant information to command line
        void print_info() const override {
            cout << "Count of TripleMeanSamples: " << countTripleMeanSamples() << endl;
            if (!samples.empty()) {
                auto bestSample = getBestSample();
                cout << "Details of sample with the max mean:" << endl;
                cout << "Data version: " << bestSample << " | ";
                cout << "Maximum value: " << *max_element(bestSample.dataVector.begin(), bestSample.dataVector.end()) << " | ";
                cout << "Mean value: " << bestSample.getStatisticsA() << endl;
            }
        }
};

//=================================================
// MUST NOT CHANGE ANYTHING IN THE MAIN FUNCTION
//=================================================
int main() 
{
    DataManager manager;
    try 
    {
        manager("v1.5.0", "wrong_input.txt");
    } 
    catch (const exception& e) 
    {
        cout << e.what() << endl << endl;
        manager("v1.5.0", "input_datarecords.txt");
        manager.print_info();
    }
    return 0;
}

