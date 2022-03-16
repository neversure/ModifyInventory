#include <iostream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>

class Item {
public:							//characteristic_number:
	std::string name;					//1
	std::string type;					//2
	int level;							//3
	std::string rarity;					//4

	Item(): level(0) {}

	Item(const std::string itemLine) {
		level = 0;

		int characteristic_number = 1;
		std::string characteristic_value;
		for (int i = 0; i < itemLine.size(); i++) {
			if (itemLine[i] != ',') characteristic_value += itemLine[i];
			else {
				switch (characteristic_number) {
				case 1:
					name = characteristic_value;
					break;
				case 2:
					type = characteristic_value;
					break;
				case 3:
					level = std::stoi(characteristic_value);
					break;
				case 4:
					rarity = characteristic_value;
					break;
				}
				characteristic_number++;
				if (characteristic_number > 4) break;									//as we need only 4 characteristics to read
				characteristic_value = "";
				continue;
			}
		}
	}
	void print() {
		std::cout << name << " " << type << " " << level << " " << rarity << " ";
	}
};

class Modificator {
public:
	std::string name;                   //1
	int filter_level;                   //-                                 //0 for any; -3 for <= 3; 4 for > 3
	std::string filter_type;            //-                                 //"ANY" for any 
	std::string filter_rarity;          //-                                 //same
	int characteristic_to_buf;          //2                                 //0 for any; 1 for damage; 2 for speed; 3 for protection;                                               
	double buf_value;                   //3                 
	Modificator(const std::string modificatorLine) {
		
		filter_level = 0;                                                   //
		buf_value = .0;
		characteristic_to_buf = -1;
		std::string characteristic_value;
		int characteristic_number = 1;
		for (int i = 0; i < modificatorLine.size(); i++) {
			if (modificatorLine[i] == '[') {                                  //as modificators should be bounded by [ ]
				while (modificatorLine[i] != ']') {
					i++;
					if (modificatorLine[i] == '{') {                          //begin to read first modificator 
						i++;
						while (modificatorLine[i] != '}') {                     //while not the end of current modificator info

							if (modificatorLine[i] != ',') characteristic_value += modificatorLine[i];
							else {
								if (characteristic_value == "level") {
									i++;
									std::string more_or_less;
									while (modificatorLine[i] != ','
										&& modificatorLine[i] != '}') more_or_less += modificatorLine[i++];

									if (more_or_less == ">=") filter_level = modificatorLine[++i] - '0';
									if (more_or_less == ">") filter_level = modificatorLine[++i] - '0' + 1;
									if (more_or_less == "<=") filter_level = -1 * (modificatorLine[++i] - '0');
									if (more_or_less == "<") filter_level = -1 * (modificatorLine[++i] - '0' + 1);
									characteristic_value = "";
									i++;
									continue;
								}
								//---------------------------------------------------------------
								if (characteristic_value == "type") {
									i++;
									if (filter_type.size() == 0) filter_type = modificatorLine.substr(i, 5);
									else filter_type = "ANY";
									i += 5;
									characteristic_value = "";
									continue;
								}
								//---------------------------------------------------------------
								if (characteristic_value == "rarity") {
									i++;
									std::string rarity;
									while (modificatorLine[i] != ','
										&& modificatorLine[i] != '}')
										rarity += modificatorLine[i++];
									filter_rarity = rarity;

									characteristic_value = "";
									continue;
								}

							}
							i++;
						}
					}
				}
				i++;
				continue;
			}
			else {
				if (modificatorLine[i] != ',' && i != modificatorLine.size() - 1) characteristic_value += modificatorLine[i];
				else {
					switch (characteristic_number) {
					case 1:
						name = characteristic_value;
						break;
					case 2:
						if (characteristic_value == "DamageBuff") characteristic_to_buf = 1;
						else if (characteristic_value == "SpeedBuff") characteristic_to_buf = 2;
						else characteristic_to_buf = 3;
						break;
					case 3:
						buf_value = atof(characteristic_value.c_str());
						break;

					}
					characteristic_number++;
					characteristic_value = "";
					continue;
				}
			}
		}
		//---------------------------------------------------------------
		if (filter_type.size() == 0) filter_type = "ANY";
		if (filter_rarity.size() == 0) filter_rarity = "ANY";

	}
	void print() {
		std::cout << name << " " << filter_level << " " << filter_type << " " << characteristic_to_buf << " " << buf_value << std::endl;
	}


};

class Weapon : public Item {
public:							//characteristic_number:
	double damage;					//1	
	double speed;					//2
	Weapon(): damage(0), speed(0) {}
	
	Weapon(Item& item, const std::string itemLine) : Item(item) {
		
		damage = .0;
		speed = .0;

		int number_of_comma_to_start = 4;
		int number_of_comma_current = 0;
		int characteristic_number = 1;
		std::string characteristic_value;

		for (int i = 0; i < itemLine.size(); i++) {
			if (itemLine[i] != ',' && i != itemLine.size() - 1) {
				if (number_of_comma_current == number_of_comma_to_start) characteristic_value += itemLine[i];
			}
			else {
				if (number_of_comma_current == number_of_comma_to_start) {
					if (i == itemLine.size() - 1) characteristic_value += itemLine[i];

					switch (characteristic_number) {
					case 1:
						damage = atof(characteristic_value.c_str());
						break;
					case 2:
						speed = atof(characteristic_value.c_str());
						break;
					}
					characteristic_number++;
					if (characteristic_number > 2) break;
					characteristic_value = "";
					continue;
				}
				else number_of_comma_current++;
			}
		}

	}
	void modify(const Modificator& modificator) {
		bool buf_level = false;
		if (modificator.filter_level == 0) buf_level = true;
		else if (modificator.filter_level > 0) {
			if (level >= modificator.filter_level) buf_level = true;
		}
		else {
			if (-1 * level >= modificator.filter_level) buf_level = true;
		}

		bool buf_type = false;
		if (modificator.filter_type == "ANY") buf_type = true;
		else if (type == modificator.filter_type) buf_type = true;

		bool buf_rarity = false;
		if (modificator.filter_rarity == "ANY") buf_rarity = true;
		else if (rarity == modificator.filter_rarity) buf_rarity = true;

		if (buf_level && buf_type && buf_rarity) {
			switch (modificator.characteristic_to_buf) {
			case 1:
				damage += modificator.buf_value;
				break;
			case 2:
				speed += modificator.buf_value;
				break;
			}
		}
	}
	void print() {
		std::cout << name << " " << type << " " << level << " " << rarity << " " << damage << " " << speed << std::endl;
	}
};

class Armour : public Item {
public:
	double protection;
	Armour(): protection(0) {}
	
	Armour(Item& item, const std::string itemLine) : Item(item) {

		protection = .0;

		int number_of_comma_to_start = 4;													//since we dont need to rewrite first four charachteristics
		int number_of_comma_current = 0;

		std::string characteristic_value;

		for (int i = 0; i < itemLine.size(); i++) {
			if (itemLine[i] != ',' && i != itemLine.size() - 1) {
				if (number_of_comma_current == number_of_comma_to_start) characteristic_value += itemLine[i];
			}
			else {
				if (number_of_comma_current == number_of_comma_to_start) {
					if (i == itemLine.size() - 1) characteristic_value += itemLine[i];			//if we on the last string char

					protection = atof(characteristic_value.c_str());
					break;
				}
				else number_of_comma_current++;
			}
		}

	}
	void modify(const Modificator& modificator) {
		if(modificator.characteristic_to_buf != 3) return;
		bool buf_level = false;
		if (modificator.filter_level == 0) buf_level = true;
		else if (modificator.filter_level > 0) {
			if (level >= modificator.filter_level) buf_level = true;
		}
		else {
			if (-1 * level >= modificator.filter_level) buf_level = true;
		}

		bool buf_type = false;
		if (modificator.filter_type == "ANY") buf_type = true;
		else if (type == modificator.filter_type) buf_type = true;

		bool buf_rarity = false;
		if (modificator.filter_rarity == "ANY") buf_rarity = true;
		else if (rarity == modificator.filter_rarity) buf_rarity = true;

		if (buf_level && buf_type && buf_rarity) {
			protection += modificator.buf_value;
		}
	}
	void print() {
		std::cout << name << " " << type << " " << level << " " << rarity << " " << protection << std::endl;
	}



};



void makeWorkData(std::string* inputData) {
	inputData->erase(std::remove(inputData->begin(), inputData->end(), ' '), inputData->end());
	inputData->erase(0, 7);
	inputData->erase(std::remove(inputData->begin(), inputData->end(), '"'), inputData->end());

	int pos_from_erase = 0;
	int pos_to_erase = 0;

	while (pos_to_erase < inputData->size() - 1) {
		if ((*inputData)[pos_to_erase] == ',') {
			pos_from_erase = pos_to_erase;
			pos_to_erase++;
			continue;
		}
		if ((*inputData)[pos_to_erase] == ':') {
			inputData->erase(pos_from_erase + 1, pos_to_erase - pos_from_erase);
			pos_from_erase++;
			pos_to_erase = pos_from_erase;

		}

		if ((*inputData)[pos_from_erase] != ',') pos_from_erase++;
		pos_to_erase++;

	}
	return;
}

class Inventory {
public:
	Weapon first_weapon;
	Weapon second_weapon;
	Weapon third_weapon;
	Weapon fourth_weapon;
	Armour first_armour;
	Armour second_armour;


	Inventory(	Weapon& _first_weapon, Weapon& _second_weapon, Weapon& _third_weapon, Weapon& _fourth_weapon,
				Armour& _first_armour, Armour& _second_armour) :
				first_weapon(_first_weapon), second_weapon(_second_weapon), third_weapon(_third_weapon), fourth_weapon(_fourth_weapon),
				first_armour(_first_armour), second_armour(_second_armour){}


	void getBuf(const Modificator& modificator) {
		first_weapon.modify(modificator);
		second_weapon.modify(modificator);
		third_weapon.modify(modificator);
		fourth_weapon.modify(modificator);
		first_armour.modify(modificator);
		second_armour.modify(modificator);

	}
	

	void print() {
		first_weapon.print();
		second_weapon.print();
		third_weapon.print();
		fourth_weapon.print();
		first_armour.print();
		second_armour.print();
	}
};

int main() {
	std::ifstream inputFile;
	inputFile.open("input.txt");
	std::vector<Weapon> weapons(4);	int w = 0;
	std::vector<Armour> armours(2); int a = 0;
	for (int i = 0; i < 6; i++) {
		std::string input_data;
		std::getline(inputFile, input_data);
		makeWorkData(&input_data);

		Item item(input_data);
		if (item.type != "Armour") {
			Weapon weapon(item, input_data);

			weapons[w] = weapon;
			w++;
			continue;
		}
		Armour armour(item, input_data);

		armours[a] = armour;
		a++;
	}
	std::cout << "------------------------------------------\n";
	Inventory inventory(weapons[0], weapons[1], weapons[2], weapons[3],
						armours[0], armours[1]);

	inventory.print();

	std::cout << "------------------------------------------\n";
	for (int i = 0; i < 3; i++) {
		std::string input_data;
		std::getline(inputFile, input_data);
		makeWorkData(&input_data);
		Modificator modificator(input_data);
		
		inventory.getBuf(modificator);
	}
	std::cout << "BuFfEd\n";
	std::cout << "------------------------------------------\n";
	inventory.print();
	std::cout << "------------------------------------------\n";

	inputFile.close();
	return 0;
}