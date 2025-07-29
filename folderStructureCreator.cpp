#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <system_error>
#include <vector>
#include <set>

using namespace std;
namespace fs = std::filesystem;

// folder paths
vector<fs::path> folder_structure = {
    "TestFolder",
    "TestFolder2/FolderContent1",
    "Hello",
    "Hello/World"
};

set<fs::path> folder_structure_set(folder_structure.begin(), folder_structure.end());

void create_folder_structure(const fs::path& base_path) {
    try {
        for (const auto& folder : folder_structure) {
            fs::path folder_path = base_path / folder;
            folder_path.make_preferred();

            if (!fs::exists(folder_path)) {
                cout << "Creating directory: " << folder_path << endl;
                error_code ec;
                if (!fs::create_directories(folder_path, ec)) {
                    cerr << "Failed to create directory: " << folder_path << ". Error: " << ec.message() << endl;
                    return;
                }
            }
            else {
                cout << "Directory already exists: " << folder_path << endl;
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "General error: " << e.what() << endl;
    }
}

void collect_empty_folders(const fs::path& folder_path, vector<fs::path>& empty_dirs) {
    try {
        for (auto it = fs::recursive_directory_iterator(folder_path, fs::directory_options::skip_permission_denied);
            it != fs::recursive_directory_iterator(); ++it) {
            try {
                const auto& dir_entry = *it;

                if (fs::is_directory(dir_entry.path()) && fs::is_empty(dir_entry.path())) {
                    empty_dirs.push_back(dir_entry.path());
                }
            }
            catch (const fs::filesystem_error& e) {
                cerr << "Filesystem error during iteration: " << e.what() << endl;
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "General error: " << e.what() << endl;
    }
}

void remove_empty_folders(const vector<fs::path>& empty_dirs) {
    for (const auto& dir : empty_dirs) {
        try {
            if (fs::exists(dir) && fs::is_empty(dir)) {
                error_code ec;
                fs::remove(dir, ec);
                if (ec) {
                    cerr << "Failed to remove directory: " << dir << ". Error: " << ec.message() << endl;
                }
                else {
                    cout << "Removed empty directory: " << dir << endl;
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            cerr << "Filesystem error: " << e.what() << endl;
        }
        catch (const exception& e) {
            cerr << "General error: " << e.what() << endl;
        }
    }
}

int main() {
    char* user_profile = nullptr;
    size_t size = 0;
    if (_dupenv_s(&user_profile, &size, "USERPROFILE") != 0 || !user_profile) {
        cerr << "Error: USERPROFILE environment variable not found." << endl;
        return 1;
    }
    fs::path download_path = fs::path(user_profile) / "Downloads";
    download_path.make_preferred();
    free(user_profile);

    cout << "Download path is: " << download_path << endl;

    string input;
    cout << "Choose an option:\n1. Create folder structure\n2. Delete empty folders\nEnter your choice (1 or 2): ";
    cin >> input;

    if (input == "1") {
        create_folder_structure(download_path);
        cout << "Folder structure creation completed." << endl;
    }
    else if (input == "2") {
        vector<fs::path> empty_dirs;
        collect_empty_folders(download_path, empty_dirs);
        remove_empty_folders(empty_dirs);
        cout << "Empty folder removal completed." << endl;
    }
    else {
        cout << "Invalid option. Please run the program again and choose 1 or 2." << endl;
    }

    cout << "Press Enter to exit...";
    cin.ignore();
    cin.get();

    return 0;
}
