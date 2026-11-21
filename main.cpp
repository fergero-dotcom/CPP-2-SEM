#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <limits>
#include <algorithm>

using namespace std;

enum HeroClass { Warrior = 1, Mage, Healer };
enum ItemType { Weapon = 1, Armor, Potion };
enum StatusEffect { None, Blessed, Cursed, Poisoned };

struct StatusFlags {
    unsigned int blessed : 1;
    unsigned int cursed : 1;
    unsigned int poisoned : 1;
};
struct WeaponStats { int damage, durability; bool twoHanded; };
struct ArmorStats { int defense, durability; bool magical; };
struct PotionStats { int healAmount, manaAmount; };
union ItemProperties {
    WeaponStats weapon;
    ArmorStats armor;
    PotionStats potion;
};
struct Item {
    string name;
    ItemType type;
    int weight, value;
    ItemProperties props;
};
struct Hero {
    string name;
    HeroClass heroClass;
    int level;
    int hp, maxHp;
    int mp, maxMp;
    StatusFlags status;
    Item* equipment;
    int equippedCount;
};
struct Enemy { string name; int hp, damage; };

int main() {
    array<Item, 16> inventory{};
    vector<Hero> heroes;
    Enemy* enemies = nullptr;
    int enemyCount = 0;
    bool running = true;

    while (running && !cin.eof()) {
        cout << "\n ТАКТИЧЕСКАЯ RPG \n"
             << "1. Отряд\n2. Инвентарь\n3. Экипировка\n"
             << "4. Враги\n5. Битва\n6. Сводка\n0. Выход\nВыбор: ";
        int section = -1;
        cin >> section;
        if (cin.eof()) break;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Введите число.\n";
            continue;
        }
        if (section == 0) break;
        if (section < 1 || section > 6) {
            cout << "Нет такого пункта.\n";
            continue;
        }

        bool submenu = true;
        while (submenu && !cin.eof()) {
            int action = 0;
            if (section <= 4) {
                if (section == 1) cout << "\nОТРЯД\n1. Добавить\n2. Удалить\n3. Показать\n";
                if (section == 2) cout << "\nИНВЕНТАРЬ\n1. Добавить\n2. Удалить\n3. Показать\n";
                if (section == 3) cout << "\nЭКИПИРОВКА\n1. Надеть\n2. Снять\n3. Показать\n";
                if (section == 4) cout << "\nВРАГИ\n1. Добавить\n2. Удалить\n3. Показать\n";
                cout << "0. Назад\nВыбор: ";
                cin >> action;
                if (cin.eof()) break;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Введите число.\n";
                    continue;
                }
                if (action == 0) break;
                if (action < 1 || action > 3) {
                    cout << "Нет такого пункта.\n";
                    continue;
                }
            } else {
                submenu = false;
            }

            switch (section) {
                case 1: {
                    if (action == 1) {
                        Hero h{};
                        int classNumber = 0, effectNumber = -1;
                        cout << "Имя: ";
                        getline(cin >> ws, h.name);
                        cout << "Класс (1 Warrior, 2 Mage, 3 Healer), уровень (1-1000),\n"
                             << "статус (0 None, 1 Blessed, 2 Cursed, 3 Poisoned): ";
                        cin >> classNumber >> h.level >> effectNumber;
                        if (!cin || h.name.empty() || classNumber < 1 || classNumber > 3 ||
                            h.level < 1 || h.level > 1000 || effectNumber < 0 || effectNumber > 3) {
                            cout << "Некорректные данные.\n";
                            break;
                        }
                        h.heroClass = static_cast<HeroClass>(classNumber);
                        switch (h.heroClass) {
                            case Warrior: h.maxHp = 100 + h.level * 20; h.maxMp = 10 + h.level * 2; break;
                            case Mage: h.maxHp = 50 + h.level * 8; h.maxMp = 100 + h.level * 25; break;
                            case Healer: h.maxHp = 60 + h.level * 10; h.maxMp = 80 + h.level * 20; break;
                        }
                        h.hp = h.maxHp;
                        h.mp = h.maxMp;
                        StatusEffect effect = static_cast<StatusEffect>(effectNumber);
                        switch (effect) {
                            case Blessed: h.status.blessed = 1; break;
                            case Cursed: h.status.cursed = 1; break;
                            case Poisoned: h.status.poisoned = 1; break;
                            case None: break;
                        }
                        heroes.push_back(h);
                        heroes.back().equipment = new Item[4]{};
                        cout << "Герой добавлен.\n";
                    }
                    if (action == 2) {
                        for (int i = 0; i < static_cast<int>(heroes.size()); i++)
                            cout << i + 1 << ". " << heroes[i].name << '\n';
                        int index = 0;
                        cout << "Номер героя (0 — отмена): ";
                        cin >> index;
                        if (!cin || index == 0) break;
                        if (index < 1 || index > static_cast<int>(heroes.size())) {
                            cout << "Неверный номер.\n"; break;
                        }
                        index--;
                        delete[] heroes[index].equipment;
                        heroes.erase(heroes.begin() + index);
                        cout << "Герой и его экипировка удалены.\n";
                    }
                    break;
                }
                case 2: {
                    if (action == 1) {
                        int slot = -1;
                        for (int i = 0; i < 16; i++)
                            if (inventory[i].name.empty()) { slot = i; break; }
                        if (slot == -1) { cout << "Инвентарь заполнен.\n"; break; }
                        Item item{};
                        int type = 0, flag = 0;
                        cout << "Название: ";
                        getline(cin >> ws, item.name);
                        cout << "Тип (1 Weapon, 2 Armor, 3 Potion), вес, цена: ";
                        cin >> type >> item.weight >> item.value;
                        if (!cin || type < 1 || type > 3 || item.weight < 0 || item.value < 0) {
                            cout << "Некорректные данные.\n"; break;
                        }
                        item.type = static_cast<ItemType>(type);
                        bool valid = true;
                        switch (item.type) {
                            case Weapon:
                                item.props.weapon = WeaponStats{};
                                cout << "Урон (0-1000000), прочность, двуручность (0/1): ";
                                cin >> item.props.weapon.damage >> item.props.weapon.durability >> flag;
                                item.props.weapon.twoHanded = flag == 1;
                                valid = item.props.weapon.damage >= 0 && item.props.weapon.damage <= 1000000 &&
                                        item.props.weapon.durability >= 0 && (flag == 0 || flag == 1);
                                break;
                            case Armor:
                                item.props.armor = ArmorStats{};
                                cout << "Защита (0-1000000), прочность, магическая (0/1): ";
                                cin >> item.props.armor.defense >> item.props.armor.durability >> flag;
                                item.props.armor.magical = flag == 1;
                                valid = item.props.armor.defense >= 0 && item.props.armor.defense <= 1000000 &&
                                        item.props.armor.durability >= 0 && (flag == 0 || flag == 1);
                                break;
                            case Potion:
                                item.props.potion = PotionStats{};
                                cout << "Восстановление HP и MP (0-1000000): ";
                                cin >> item.props.potion.healAmount >> item.props.potion.manaAmount;
                                valid = item.props.potion.healAmount >= 0 && item.props.potion.healAmount <= 1000000 &&
                                        item.props.potion.manaAmount >= 0 && item.props.potion.manaAmount <= 1000000;
                                break;
                        }
                        if (cin && valid) { inventory[slot] = item; cout << "Предмет добавлен.\n"; }
                        else cout << "Некорректные характеристики.\n";
                    }
                    if (action == 2) {
                        int slot = 0;
                        cout << "Номер слота (1-16, 0 — отмена): ";
                        cin >> slot;
                        if (!cin || slot == 0) break;
                        if (slot < 1 || slot > 16) { cout << "Неверный номер.\n"; break; }
                        inventory[slot - 1] = Item{};
                        cout << "Слот очищен.\n";
                    }
                    break;
                }
                case 3: {
                    if (heroes.empty()) { cout << "Отряд пуст.\n"; break; }
                    for (int i = 0; i < static_cast<int>(heroes.size()); i++)
                        cout << i + 1 << ". " << heroes[i].name << '\n';
                    int index = 0;
                    cout << "Номер героя (0 — отмена): ";
                    cin >> index;
                    if (!cin || index == 0) break;
                    if (index < 1 || index > static_cast<int>(heroes.size())) {
                        cout << "Неверный номер.\n"; break;
                    }
                    Hero& h = heroes[index - 1];
                    if (action == 1) {
                        int slot = -1;
                        for (int i = 0; i < 4; i++)
                            if (h.equipment[i].name.empty()) { slot = i; break; }
                        if (slot == -1) { cout << "Все 4 слота заняты.\n"; break; }
                        for (int i = 0; i < 16; i++)
                            if (!inventory[i].name.empty()) cout << i + 1 << ". " << inventory[i].name << '\n';
                        int itemIndex = 0;
                        cout << "Номер предмета (0 — отмена): ";
                        cin >> itemIndex;
                        if (!cin || itemIndex == 0) break;
                        if (itemIndex < 1 || itemIndex > 16) { cout << "Неверный номер.\n"; break; }
                        if (inventory[itemIndex - 1].name.empty()) { cout << "Слот пуст.\n"; break; }
                        h.equipment[slot] = inventory[itemIndex - 1];
                        inventory[itemIndex - 1] = Item{};
                        h.equippedCount++;
                        cout << "Предмет надет в слот " << slot + 1 << ".\n";
                    } else {
                        for (int i = 0; i < 4; i++)
                            cout << i + 1 << ". " << (h.equipment[i].name.empty() ? "Пусто" : h.equipment[i].name) << '\n';
                        if (action == 2) {
                            int slot = 0;
                            cout << "Номер слота (0 — отмена): ";
                            cin >> slot;
                            if (!cin || slot == 0) break;
                            if (slot < 1 || slot > 4) { cout << "Неверный номер.\n"; break; }
                            if (h.equipment[slot - 1].name.empty()) { cout << "Слот пуст.\n"; break; }
                            int freeSlot = -1;
                            for (int i = 0; i < 16; i++)
                                if (inventory[i].name.empty()) { freeSlot = i; break; }
                            if (freeSlot == -1) { cout << "Инвентарь заполнен.\n"; break; }
                            inventory[freeSlot] = h.equipment[slot - 1];
                            h.equipment[slot - 1] = Item{};
                            h.equippedCount--;
                            cout << "Предмет снят.\n";
                        }
                    }
                    break;
                }
                case 4: {
                    if (action == 1) {
                        Enemy enemy{};
                        char nameBuffer[100]{};
                        cout << "Имя врага (до 99 байт): ";
                        cin >> ws;
                        cin.getline(nameBuffer, 100);
                        if (!cin) { cout << "Имя слишком длинное или ввод прерван.\n"; break; }
                        enemy.name = string(nameBuffer);
                        cout << "HP (1-1000000), урон (0-1000000): ";
                        cin >> enemy.hp >> enemy.damage;
                        if (!cin || enemy.hp < 1 || enemy.hp > 1000000 || enemy.damage < 0 || enemy.damage > 1000000) {
                            cout << "Некорректные данные.\n"; break;
                        }
                        Enemy* newArray = new Enemy[enemyCount + 1];
                        for (int i = 0; i < enemyCount; i++) newArray[i] = enemies[i];
                        newArray[enemyCount] = enemy;
                        delete[] enemies;
                        enemies = newArray;
                        enemyCount++;
                        cout << "Враг добавлен.\n";
                    }
                    if (action == 2) {
                        for (int i = 0; i < enemyCount; i++) cout << i + 1 << ". " << enemies[i].name << '\n';
                        int index = 0;
                        cout << "Номер врага (0 — отмена): ";
                        cin >> index;
                        if (!cin || index == 0) break;
                        if (index < 1 || index > enemyCount) { cout << "Неверный номер.\n"; break; }
                        Enemy* newArray = nullptr;
                        if (enemyCount > 1) newArray = new Enemy[enemyCount - 1];
                        int j = 0;
                        for (int i = 0; i < enemyCount; i++)
                            if (i != index - 1) newArray[j++] = enemies[i];
                        delete[] enemies;
                        enemies = newArray;
                        enemyCount--;
                        cout << "Враг удалён.\n";
                    }
                    break;
                }
                case 5: {
                    int round = 1;
                    bool interrupted = false;
                    for (int i = 0; i < static_cast<int>(heroes.size()); i++)
                        cout << "Герой " << i + 1 << ": " << heroes[i].name << " HP=" << heroes[i].hp << '\n';
                    for (int i = 0; i < enemyCount; i++)
                        cout << "Враг " << i + 1 << ": " << enemies[i].name << " HP=" << enemies[i].hp << '\n';
                    while (true) {
                        int aliveHeroes = 0, aliveEnemies = 0;
                        for (const Hero& h : heroes) if (h.hp > 0) aliveHeroes++;
                        for (int i = 0; i < enemyCount; i++) if (enemies[i].hp > 0) aliveEnemies++;
                        if (aliveHeroes == 0 || aliveEnemies == 0) {
                            if (round == 1) cout << "Для битвы нужны живые герои и враги.\n";
                            else cout << (aliveHeroes > 0 ? "Победа!\n" : "Поражение!\n");
                            break;
                        }
                        cout << "\n===== РАУНД " << round << " =====\n";
                        for (Hero& h : heroes) {
                            if (h.hp > 0 && h.status.poisoned) {
                                h.hp = max(0, h.hp - 3);
                                cout << h.name << ": 3 урона от яда.\n";
                            }
                        }
                        for (Hero& h : heroes) {
                            if (h.hp <= 0) continue;
                            aliveEnemies = 0;
                            for (int i = 0; i < enemyCount; i++) if (enemies[i].hp > 0) aliveEnemies++;
                            if (aliveEnemies == 0) break;
                            cout << "\nХод: " << h.name << " HP=" << h.hp << " MP=" << h.mp << '\n';
                            Item& potion = h.equipment[3];
                            if (!potion.name.empty() && potion.type == Potion) {
                                int drink = 0;
                                do {
                                    cout << "Выпить зелье из 4-го слота? (0/1): ";
                                    cin >> drink;
                                    if (!cin) break;
                                } while (drink != 0 && drink != 1);
                                if (!cin) { interrupted = true; break; }
                                if (drink == 1) {
                                    h.hp = min(h.maxHp, h.hp + potion.props.potion.healAmount);
                                    h.mp = min(h.maxMp, h.mp + potion.props.potion.manaAmount);
                                    potion = Item{};
                                    h.equippedCount--;
                                    cout << "Зелье выпито. HP=" << h.hp << " MP=" << h.mp << '\n';
                                }
                            }
                            int damage = 0, cost = 0;
                            switch (h.heroClass) {
                                case Warrior: damage = 15; cost = 20; cout << "Боевой клич: урон x1.5.\n"; break;
                                case Mage: damage = 8; cost = 40; cout << "Чароплёт: урон x3.\n"; break;
                                case Healer: damage = 6; cost = 40; cout << "Лечение союзника.\n"; break;
                            }
                            for (int i = 0; i < 4; i++)
                                if (!h.equipment[i].name.empty() && h.equipment[i].type == Weapon)
                                    damage += h.equipment[i].props.weapon.damage;
                            int ability = 0;
                            if (h.mp >= cost) {
                                do {
                                    cout << "Способность за " << cost << " MP? (0/1): ";
                                    cin >> ability;
                                    if (!cin) break;
                                } while (ability != 0 && ability != 1);
                            } else cout << "Мало маны: обычная атака.\n";
                            if (!cin) { interrupted = true; break; }
                            int target = 0;
                            if (ability == 1 && h.heroClass == Healer) {
                                for (int i = 0; i < static_cast<int>(heroes.size()); i++)
                                    cout << i + 1 << ". " << heroes[i].name << " HP=" << heroes[i].hp << '\n';
                                do {
                                    cout << "Номер живого союзника: ";
                                    cin >> target;
                                    if (!cin) break;
                                } while (target < 1 || target > static_cast<int>(heroes.size()) || heroes[target - 1].hp <= 0);
                                if (!cin) { interrupted = true; break; }
                                Hero& ally = heroes[target - 1];
                                int oldHp = ally.hp;
                                ally.hp = min(ally.maxHp, ally.hp + damage);
                                h.mp -= cost;
                                cout << "Восстановлено " << ally.hp - oldHp << " HP.\n";
                            } else {
                                for (int i = 0; i < enemyCount; i++)
                                    cout << i + 1 << ". " << enemies[i].name << " HP=" << enemies[i].hp << '\n';
                                do {
                                    cout << "Номер живого врага: ";
                                    cin >> target;
                                    if (!cin) break;
                                } while (target < 1 || target > enemyCount || enemies[target - 1].hp <= 0);
                                if (!cin) { interrupted = true; break; }
                                if (ability == 1) {
                                    h.mp -= cost;
                                    if (h.heroClass == Warrior) damage = damage * 3 / 2;
                                    if (h.heroClass == Mage) damage *= 3;
                                }
                                if (h.status.blessed) damage *= 2;
                                if (h.status.cursed) damage = (damage + 1) / 2;
                                enemies[target - 1].hp = max(0, enemies[target - 1].hp - damage);
                                cout << "Нанесено " << damage << " урона.\n";
                            }
                        }
                        if (interrupted) break;
                        for (int i = 0; i < enemyCount; i++) {
                            if (enemies[i].hp <= 0) continue;
                            aliveHeroes = 0;
                            for (const Hero& h : heroes) if (h.hp > 0) aliveHeroes++;
                            if (aliveHeroes == 0) break;
                            cout << "\nАтакует " << enemies[i].name << '\n';
                            for (int j = 0; j < static_cast<int>(heroes.size()); j++)
                                cout << j + 1 << ". " << heroes[j].name << " HP=" << heroes[j].hp << '\n';
                            int target = 0;
                            do {
                                cout << "Номер живого героя: ";
                                cin >> target;
                                if (!cin) break;
                            } while (target < 1 || target > static_cast<int>(heroes.size()) || heroes[target - 1].hp <= 0);
                            if (!cin) { interrupted = true; break; }
                            Hero& h = heroes[target - 1];
                            int defense = 0;
                            for (int j = 0; j < 4; j++)
                                if (!h.equipment[j].name.empty() && h.equipment[j].type == Armor)
                                    defense += h.equipment[j].props.armor.defense;
                            int damage = max(1, enemies[i].damage - defense);
                            h.hp = max(0, h.hp - damage);
                            cout << h.name << " получает " << damage << " урона.\n";
                        }
                        if (interrupted) break;
                        cout << "\nСостояние после раунда:\n";
                        for (const Hero& h : heroes) {
                            cout << h.name << (h.hp > 0 ? " [жив]" : " [мёртв]")
                                 << " HP=" << h.hp << '/' << h.maxHp << " MP=" << h.mp << '/' << h.maxMp << " Статусы: ";
                            if (h.status.blessed) cout << "Blessed ";
                            if (h.status.cursed) cout << "Cursed ";
                            if (h.status.poisoned) cout << "Poisoned ";
                            if (!h.status.blessed && !h.status.cursed && !h.status.poisoned) cout << "None";
                            cout << '\n';
                        }
                        for (int i = 0; i < enemyCount; i++)
                            cout << enemies[i].name << " HP=" << enemies[i].hp
                                 << (enemies[i].hp > 0 ? " [жив]\n" : " [мёртв]\n");
                        round++;
                    }
                    if (interrupted) cout << "Битва прервана из-за ошибки ввода. Изменения сохранены.\n";
                    break;
                }
                case 6:
                    cout << "\n===== СВОДКА =====\n";
                    break;
            }

            if ((section == 1 && action == 3) || section == 6) {
                long long levels = 0, hp = 0, mp = 0;
                cout << "\nГерои:\n";
                if (heroes.empty()) cout << "Отряд пуст.\n";
                for (int i = 0; i < static_cast<int>(heroes.size()); i++) {
                    Hero& h = heroes[i];
                    levels += h.level; hp += h.hp; mp += h.mp;
                    cout << i + 1 << ". " << h.name << " • ";
                    switch (h.heroClass) {
                        case Warrior: cout << "Warrior"; break;
                        case Mage: cout << "Mage"; break;
                        case Healer: cout << "Healer"; break;
                    }
                    cout << " • Уровень: " << h.level << " • HP: " << h.hp << '/' << h.maxHp
                         << " • MP: " << h.mp << '/' << h.maxMp << " • Статусы: ";
                    if (h.status.blessed) cout << "Blessed ";
                    if (h.status.cursed) cout << "Cursed ";
                    if (h.status.poisoned) cout << "Poisoned ";
                    if (!h.status.blessed && !h.status.cursed && !h.status.poisoned) cout << "None";
                    cout << '\n';
                    if (section == 6) {
                        cout << "Экипировка (" << h.equippedCount << "/4):\n";
                        for (int j = 0; j < 4; j++)
                            cout << "  " << j + 1 << ". " << (h.equipment[j].name.empty() ? "Пусто" : h.equipment[j].name) << '\n';
                    }
                }
                if (section == 6)
                    cout << "Героев: " << heroes.size() << " • Общий уровень: " << levels
                         << " • Всего HP: " << hp << " • Всего MP: " << mp << '\n';
            }
            if ((section == 2 && action == 3) || section == 6) {
                int occupied = 0;
                long long weight = 0, value = 0;
                cout << "\nИнвентарь:\n";
                for (int i = 0; i < 16; i++) {
                    const Item& item = inventory[i];
                    cout << i + 1 << ". ";
                    if (item.name.empty()) { cout << "Пусто\n"; continue; }
                    occupied++; weight += item.weight; value += item.value;
                    cout << item.name << " • Вес: " << item.weight << " • Цена: " << item.value;
                    switch (item.type) {
                        case Weapon:
                            cout << " • Weapon • Урон: " << item.props.weapon.damage
                                 << " • Прочность: " << item.props.weapon.durability
                                 << " • Двуручное: " << item.props.weapon.twoHanded; break;
                        case Armor:
                            cout << " • Armor • Защита: " << item.props.armor.defense
                                 << " • Прочность: " << item.props.armor.durability
                                 << " • Магическая: " << item.props.armor.magical; break;
                        case Potion:
                            cout << " • Potion • HP: " << item.props.potion.healAmount
                                 << " • MP: " << item.props.potion.manaAmount; break;
                    }
                    cout << '\n';
                }
                cout << "Занято: " << occupied << "/16 • Вес: " << weight << " • Стоимость: " << value << '\n';
            }
            if ((section == 4 && action == 3) || section == 6) {
                cout << "\nВраги:\n";
                if (enemyCount == 0) cout << "Врагов нет.\n";
                for (int i = 0; i < enemyCount; i++)
                    cout << i + 1 << ". " << enemies[i].name << " • HP: " << enemies[i].hp
                         << " • Урон: " << enemies[i].damage << '\n';
            }
            if (cin.fail() && !cin.eof()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Ошибка ввода. Действие отменено или прервано.\n";
            }
        }
    }
    for (Hero& h : heroes) delete[] h.equipment;
    delete[] enemies;
    cout << "Программа завершена.\n";
    return 0;
}
