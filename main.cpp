#include <iostream>
#include "simple-db.cpp"

struct MyStruct {
    int id;
    float value;
};

char resolveAction() {
    char input;
    std::cout << "Select an action:" << std::endl;
    std::cout << "1. Add item" << std::endl;
    std::cout << "2. Delete item" << std::endl;
    std::cout << "3. List items" << std::endl;
    std::cout << "4. Filter items" << std::endl;
    std::cout << "5. Sort items" << std::endl;
    std::cout << "e. Exit" << std::endl;
    std::cout << "Select your option: ";
    std::cin >> input;
    std::cin.ignore();

    return input;
}

void handleAddAction(std::vector<MyStruct>& data) {
    MyStruct item{};
    std::cout << "Enter ID: ";
    std::cin >> item.id;
    std::cout << "Enter value: ";
    std::cin >> item.value;

    data.push_back(item);
    // !!! IMPORTANT: Serialize the data after adding an item
    SimpleDB::serialize(data);

    std::cout << "Item added successfully." << std::endl;
}

void showItem(const MyStruct item) {
    std::cout << "ID: " << item.id << ", Value: " << item.value << std::endl;
}

void handleShowAction(std::vector<MyStruct>& data) {
    for (const MyStruct& item : data) {
        showItem(item);
    }
}

void handleDeleteAction(std::vector<MyStruct>& data) {
    // TODO: Implement
    // !!! IMPORTANT: Serialize the data after deleting an item
    SimpleDB::serialize(data);
}

void handleFilterAction(std::vector<MyStruct>& data) {
    std::vector<MyStruct> filteredData;
    // TODO: Implement
}

void handleSortAction(std::vector<MyStruct>& data) {
    // TODO: Implement
}

int main() {
    std::vector<MyStruct> data = SimpleDB::load<MyStruct>();

    while (true) {
        switch (resolveAction()) {
            case '1':
                handleAddAction(data);
                break;
            case '2':
                handleDeleteAction(data);
                break;
            case '3':
                handleShowAction(data);
                break;
            case '4':
                handleFilterAction(data);
                break;
            case '5':
                handleSortAction(data);
                break;
            case 'e':
                std::cout << "Exiting..." << std::endl;
                return 0;
            default:
                std::cout << "Invalid action. Try again." << std::endl;
        }
        std::cout << "\n\n------------------------------------------\n\n";
    }
}
