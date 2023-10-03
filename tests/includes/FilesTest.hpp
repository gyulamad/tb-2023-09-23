#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include "../../src/includes/Files.hpp"

using namespace std;

class FilesTest {
protected:

    // Helper function to create test files
    static void createTestFiles() {
        filesystem::create_directory("testfiles");
        ofstream file1("testfiles/file1.jpg");
        ofstream file2("testfiles/file2.jpg");
        ofstream file3("testfiles/file3.txt");
        file1.close();
        file2.close();
        file3.close();
    }

    // Helper function to remove test files and the testfiles directory
    static void cleanupTestFiles() {
        filesystem::remove_all("testfiles");
    }

public:

    static void testFiles_findByExtension() {
        // Create some test files in the "testfiles" folder
        createTestFiles();

        // Test finding files by extension
        vector<string> jpgFiles = Files::findByExtension("testfiles", "jpg");
        assert(jpgFiles.size() == 2); // Assuming you have two .jpg files in "testfiles"

        vector<string> txtFiles = Files::findByExtension("testfiles", "txt");
        assert(txtFiles.size() == 1); // Assuming you have one .txt file in "testfiles"

        // Clean up: Remove the test files
        cleanupTestFiles();
    }

    static void testFiles_findByExtensions() {
        // Create some test files in the "testfiles" folder
        createTestFiles();

        // Test finding files by multiple extensions
        vector<string> extensions = { "jpg", "txt" };
        vector<string> jpgAndTxtFiles = Files::findByExtensions("testfiles", extensions);
        assert(jpgAndTxtFiles.size() == 3); // Assuming you have three .jpg and .txt files in "testfiles"

        // Clean up: Remove the test files
        cleanupTestFiles();
    }

    static void testFiles_replaceExtension() {
        // Test replacing extension
        assert(Files::replaceExtension("file.txt", "jpg") == "file.jpg");

        // Test with no extension
        assert(Files::replaceExtension("file", "jpg") == "file.jpg");

        // Test with empty filename
        assert(Files::replaceExtension("", "jpg") == ".jpg");

        // Test replacing extension with an empty one
        assert(Files::replaceExtension("file.txt", "") == "file.");
    }

    static void testFiles_normalizePath() {
        // Test normalizing path
        assert(Files::normalizePath("path/to/../file.txt") == "path/file.txt");
        assert(Files::normalizePath("../path/to/file.txt") == "../path/to/file.txt");
        assert(Files::normalizePath("./path/to/file.txt") == "path/to/file.txt");
    }

    static void testFiles_extractPath() {
        // Test extracting path
        assert(Files::extractPath("path/to/file.txt") == "path/to");
        assert(Files::extractPath("file.txt") == "");
        assert(Files::extractPath("") == "");
    }

    static void testFiles_exists() {
        // Create some test files in the "testfiles" folder
        createTestFiles();

        // Test file existence
        assert(Files::exists("testfiles/file1.jpg") == true);
        assert(Files::exists("nonexistent.txt") == false);

        // Clean up: Remove the test files
        cleanupTestFiles();
    }

    static void testFiles_createPath() {
        // Test creating a directory
        string testDir = "test_directory";
        assert(Files::exists(testDir) == false);
        Files::createPath(testDir);
        assert(Files::exists(testDir) == true);

        // Clean up: Remove the created directory
        filesystem::remove_all(testDir);
    }

    static void testFiles_getLastModificationTime() {
        // Test getting last modification time
        string testFile = "test.txt";
        // Create a test file (touch it)
        ofstream outfile(testFile);
        outfile.close();

        time_t lastModTime = Files::getLastModificationTime(testFile);
        // Ensure the last modification time is not 0 (epoch)
        assert(lastModTime != 0);

        // Clean up: Remove the test file
        filesystem::remove(testFile);
    }

    static void testFiles_file_get_contents() {
        // Create a test file with known content
        const string filename = "test_file_get_contents.txt";
        const string expected_content = "This is a test content.";

        // Write test content to the file
        Files::file_put_contents(filename, expected_content);

        // Read the content using file_get_contents
        string actual_content;
        try {
            actual_content = Files::file_get_contents(filename);
        } catch (const exception& e) {
            cerr << "Error in testFiles_file_get_contents: " << e.what() << endl;
            return;
        }

        // Check if the actual content matches the expected content
        assert(actual_content == expected_content);

        // Clean up: remove the test file
        remove(filename.c_str());
    }

    static void testFiles_file_put_contents() {
        // Define test data
        const string filename = "test_file_put_contents.txt";
        const string test_content = "This is a test content.";

        // Write test content to the file using file_put_contents
        try {
            Files::file_put_contents(filename, test_content);
        } catch (const exception& e) {
            cerr << "Error in testFiles_file_put_contents: " << e.what() << endl;
            return;
        }

        // Read the content of the file
        ifstream file(filename);
        string actual_content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
        file.close();

        // Check if the actual content matches the expected content
        assert(actual_content == test_content);

        // Clean up: remove the test file
        remove(filename.c_str());
    }
};