#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>

int size_map = 0;

struct Segment
{
	int x;
	int y;
    Segment* next;
};
class Snake
{
public:
    Segment* head;
    Snake()
    {
        head = nullptr;
    }
	~Snake()
	{
		clear();
	}
    void addHead(int x, int y) 
    {
        Segment* snake = new Segment;
        snake->x = x;
        snake->y = y;
        snake->next = head;
        head = snake;
    }
    void addTail(int x, int y) 
    {
        Segment* snake = new Segment;
        snake->x = x;
        snake->y = y;
        snake->next = nullptr;

        if (head == nullptr) 
        {
            head = snake;
            return;
        }

        Segment* s = head;
        while (s->next != nullptr)
            s = s->next;
        s->next = snake;
    }
    void removeTail()
    {
        if (head == nullptr) return;

        if (head->next == nullptr) 
        {

            delete head;
            head = nullptr;
            return;
        }

        Segment* s = head;
        while (s->next->next != nullptr)
            s = s->next;

        delete s->next;
        s->next = nullptr;
    }
    void clear() 
    {
        while (head != nullptr)
        {
            Segment* s = head;
            head = head->next;
            delete s;
        }
    }
    int length() const
    {
        int count = 0;
        for (Segment* c = head; c != nullptr; c = c->next)
            count++;
        return count;
    }
    bool contains(int x, int y)
    {
        for (Segment* s = head; s != nullptr; s = s->next)
            if (s->x == x && s->y == y)
                return true;
        return false;
    }

};
struct Coord
{
    int x;
    int y;
};
struct SaveHeader 
{
    int size;
    int score;
    int direction;
    int foodX;
    int foodY;
    int length;
};
struct Record 
{
    char name[20];
    int score;
};
struct Game 
{
    Snake snake;
    int score;
    int direction;
    int foodX;
    int foodY;
};
void placeFood(Game& game) 
{
    do 
    {
        game.foodX = rand() % size_map;
        game.foodY = rand() % size_map;
    } while (game.snake.contains(game.foodX, game.foodY));
}
void initGame(Game& game) 
{
    int right = 1;

    game.snake.clear();
    game.snake.addHead(size_map / 2, size_map / 2);
    game.score = 0;
    game.direction = right;
    placeFood(game);
}
void draw(Game& game) 
{
    system("cls");

    std::cout << "Счет: " << game.score << "   Длина: " << game.snake.length() << std::endl;

    for (int i = 0; i < size_map * 2 + 2; i++)
        std::cout << '*';
    std::cout << std::endl;

    for (int y = 0; y < size_map; y++)
    {
        std::cout << '*';
        for (int x = 0; x < size_map; x++)
        {
            if (game.snake.head != nullptr && game.snake.head->x == x && game.snake.head->y == y)
                std::cout << "O ";
            else if (game.snake.contains(x, y))
                std::cout << "o ";
            else if (game.foodX == x && game.foodY == y)
                std::cout << "$ ";
            else
                std::cout << "  ";
        }
        std::cout << '*' << std::endl;
    }

    for (int i = 0; i < size_map * 2 + 2; i++)
        std::cout << '*';
    std::cout << std::endl;
}
void saveGame(Game& game) 
{
    std::ofstream file("SAVE_FILE", std::ios::binary);
    if (!file)
    {
        std::cout << "Не удалось сохранить игру" << std::endl;
        return;
    }

    SaveHeader h;
    h.size = size_map;
    h.score = game.score;
    h.direction = game.direction;
    h.foodX = game.foodX;
    h.foodY = game.foodY;
    h.length = game.snake.length();

    file.write(reinterpret_cast<const char*>(&h), sizeof(h));

    for (Segment* s = game.snake.head; s != nullptr; s = s->next) 
    {
        Coord p;
        p.x = s->x;
        p.y = s->y;
        file.write(reinterpret_cast<const char*>(&p), sizeof(p));
    }

    file.close();
}
bool loadGame(Game& game) 
{
    std::ifstream file("SAVE_FILE", std::ios::binary);
    if (!file) return false;

    SaveHeader h;
    file.read(reinterpret_cast<char*>(&h), sizeof(h));

    if(!file || h.size < 10 || h.size > 30 ||
                h.direction < 0 || h.direction > 3 ||
                h.length <= 0 || h.length > h.size * h.size ||
                h.foodX < 0 || h.foodX >= h.size ||
                h.foodY < 0 || h.foodY >= h.size)
        return false;

    game.snake.clear();
    for (int i = 0; i < h.length; i++) 
    {
        Coord p;
        file.read(reinterpret_cast<char*>(&p), sizeof(p));
        if (!file || p.x < 0 || p.x >= h.size || p.y < 0 || p.y >= h.size)
        {
            game.snake.clear();
            return false;
        }
        game.snake.addTail(p.x, p.y);
    }

    size_map = h.size;
    game.score = h.score;
    game.direction = h.direction;
    game.foodX = h.foodX;
    game.foodY = h.foodY;

    file.close();
    return true;
}
void addRecord(char* name, int score) 
{
    Record r;

    for (int i = 0; i < 20; i++)
        r.name[i] = '\0';

    int i = 0;
    while (i < 19 && name[i] != '\0') 
    {
        r.name[i] = name[i];
        i++;
    }

    r.score = score;

    std::ofstream file("RECORDS_FILE", std::ios::binary | std::ios::app);
    if (file)
        file.write(reinterpret_cast<const char*>(&r), sizeof(r));
}

const int MAX_RECORDS = 1000;

void showRecords() 
{
    system("cls");
    std::cout << "Таблица рекордов" << std::endl;

    Record list[MAX_RECORDS];
    int count = 0;

    std::ifstream file("RECORDS_FILE", std::ios::binary);
    while (count < MAX_RECORDS && file.read(reinterpret_cast<char*>(&list[count]), sizeof(Record)))
        count++;
    file.close();

    if (count == 0) 
    {
        std::cout << "Пока нет рекордов" << std::endl;
    }
    else
    {
        for (int i = 0; i < count - 1; i++)
        {
            for (int j = 0; j < count - 1 - i; j++) 
            {
                if (list[j].score < list[j + 1].score) 
                {
                    Record tmp = list[j];
                    list[j] = list[j + 1];
                    list[j + 1] = tmp;
                }
            }
        }

        for (int i = 0; i < count && i < 10; i++)
            std::cout << i + 1 << ". " << list[i].name << " - " << list[i].score << std::endl;
    }

    std::cout << std::endl << "Нажмите любую клавишу...";
    _getch();
}

const int DX[4] = { 0, 1, 0, -1 };
const int DY[4] = { -1, 0, 1, 0 };

void play(Game& game)
{
    int up = 0;
    int right = 1; 
    int down = 2;
    int left = 3;

    bool gameOver = false;
    bool won = false;

    while (true)
    {
        draw(game);
        int prevDir = game.direction;
        while (_kbhit()) 
        {
            int key = _getch();
            int newDir = -1;

            if (key == 'w' || key == 'W') newDir = up;
            else if (key == 'd' || key == 'D') newDir = right;
            else if (key == 's' || key == 'S') newDir = down;
            else if (key == 'a' || key == 'A') newDir = left;
            else if (key == 'k' || key == 'K') 
            {
                saveGame(game);
                std::cout << "Игра сохранена!" << std::endl;
                Sleep(700);
            }
            else if (key == 'q' || key == 'Q')
            {
                return;
            }

            if (newDir != -1 && (newDir + 2) % 4 != prevDir)
                game.direction = newDir;
        }

        int nx = game.snake.head->x + DX[game.direction];
        int ny = game.snake.head->y + DY[game.direction];

        if (nx < 0 || nx >= size_map || ny < 0 || ny >= size_map)
        {
            gameOver = true;
            break;
        }

        bool eat = (nx == game.foodX && ny == game.foodY);

        if (!eat)
            game.snake.removeTail();

        if (game.snake.contains(nx, ny))
        {
            gameOver = true;
            break;
        }

        game.snake.addHead(nx, ny);

        if (eat)
        {
            game.score += 10;
            placeFood(game);
        }

        Sleep(200);
    }
    if (gameOver)
    {
        draw(game);
        if (won)
            std::cout << std::endl << "ПОБЕДА! Вы заполнили всё поле! Ваш счет: " << game.score << std::endl;
        else
            std::cout << std::endl << "ИГРА ОКОНЧЕНА! Ваш счет: " << game.score << std::endl;

        while (_kbhit())
            _getch();

        std::cout << "Введите имя игрока: ";

        char name[20];
        std::cin.getline(name, 20);

        if (!std::cin)
        {
            std::cin.clear();
            std::cin.ignore();
        }

        if (name[0] == '\0')
        {
            const char* def = "Игрок";
            int i = 0;
            while (def[i] != '\0' && i < 19)
            {
                name[i] = def[i];
                i++;
            }
            name[i] = '\0';
        }
        addRecord(name, game.score);
    }
}
bool sizeMap()
{
    std::cout << "Введите размер поля (10-30): ";

    if (!(std::cin >> size_map))
    {
        std::cin.clear();
        std::cin.ignore();
        std::cout << "Нужно ввести число" << std::endl;
        return false;
    }
    std::cin.ignore();

    if (size_map < 10)
    {
        std::cout << "Минимальный размер поля 10x10" << std::endl;
        return false;
    }
    if (size_map > 30)
    {
        std::cout << "Максимальный размер поля 30x30" << std::endl;
        return false;
    }
    return true;
}
int main()
{
    setlocale(LC_ALL,"Russian");
    srand(time(NULL));

    Game game;
    int choice = 0;

    while (choice != 4)
    {
        system("cls");
        std::cout << "ЗМЕЙКА" << std::endl;
        std::cout << "1.Новая игра" << std::endl;
        std::cout << "2.Загрузить игру" << std::endl;
        std::cout << "3.Таблица рекордов" << std::endl;
        std::cout << "4.Выход" << std::endl;
        std::cout << "Ваш выбор: ";
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) 
        {
            if (!sizeMap())
            {
                return 1;
            }
            initGame(game);
            play(game);
        }
        else if (choice == 2) 
        {
            if (loadGame(game))
            {
                play(game);
            }
            else 
            {
                std::cout << "Сохранение не найдено или повреждено" << std::endl;
                Sleep(1500);
            }
        }
        else if (choice == 3)
        {
            showRecords();
        }
    }

    return 0;
}