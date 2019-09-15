#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <cstdlib>
#include <queue>
#include <thread>
#include <experimental/optional>
#include <cstdio>

class Stone {
    
public:
    Stone(): mask_{} {}
    Stone(const std::array<uint8_t, 4*4> l) : mask_{l} {}
    
    Stone(const Stone& other) =default;
    
    Stone& operator=(Stone& other) =default;
    
    const std::array<uint8_t, 4*4>& mask() const noexcept {
        return mask_;
    }
    Stone rotateLeft() const noexcept {
        std::array<uint8_t, 4*4> rotated_mask {};
        const auto& current_mask = mask();
        for (auto row = 0; row < 4; row++) {
            for (auto col = 0; col < 4; col++) {
                rotated_mask[row*4+col] = current_mask[col*4 + 3 - row];
            }
        }
        return Stone(rotated_mask);
    }

    Stone rotateRight() const noexcept {
        std::array<uint8_t, 4*4> rotated_mask {};
        const auto& current_mask = mask();
        for (auto row = 0; row < 4; row++) {
            for (auto col = 0; col < 4; col++) {
                rotated_mask[row*4+col] = current_mask[12 - 4 * col + row];
            }
        }
        return Stone(rotated_mask);
    }
    

private:
    std::array<uint8_t, 4*4> mask_;
};


Stone stone_cube {
    {0,0,0,0,
     0,1,1,0,
     0,1,1,0,
     0,0,0,0}};

Stone stone_s {
    {0,0,0,0,
     0,3,3,0,
     3,3,0,0,
     0,0,0,0}};

Stone stone_z {
    {0,0,0,0,
     4,4,0,0,
     0,4,4,0,
     0,0,0,0}};

Stone stone_i {
    {0,0,5,0,
     0,0,5,0,
     0,0,5,0,
     0,0,5,0}};

Stone stone_l1 {
    {0,0,0,0,
     0,6,0,0,
     0,6,0,0,
     0,6,6,0}};

Stone stone_l2 {
    {0,0,0,0,
     0,0,7,0,
     0,0,7,0,
     0,7,7,0}};



std::vector<Stone> stones {stone_cube, stone_s, stone_z, stone_i, stone_l1, stone_l2};

template<int WIDTH, int HEIGHT>
class Playground {
    
public:
    
    Playground() {
        int k {0};
        for (auto elem = data_.begin(); elem != data_.end(); ++elem, ++k) {
            if (k % WIDTH <= 1 || k % WIDTH >= WIDTH - 2) {
                *elem = 2;
            }
            if (k/WIDTH >= HEIGHT - 2) {
                *elem = 2;
            }
        }
        newStone();
    }
    
    void display() {
        
        // Clear screen, cursor at 1,1
        std::cout << "\033[2J" << "\033[1;1H";
        
        int col = 0;
        int row = 1;
        int k = 0;
        
        for (auto elem = data_.cbegin(); elem != data_.cend(); ++elem) {

            int c = colFromStone(k);
            int r = rowFromStone(k);
            bool in_stone = 0 <= c && c < 4 && 0 <= r && r < 4;
            auto current = in_stone && current_stone_.mask()[r * 4 + c] != 0 ? current_stone_.mask()[r * 4 + c] : *elem;
            switch (current) {
                case 0:
                    std::cout << ' ';
                    break;
                case 2:
                    std::cout << "\033[30m" << "#";
                    break;
                case 3:
                    std::cout << "\033[31m" << "X";
                    break;
                case 4:
                    std::cout << "\033[32m" << "X";
                    break;
                case 5:
                    std::cout << "\033[34m" << "X";
                    break;
                case 6:
                    std::cout << "\033[35m" << "X";
                    break;
                case 7:
                    std::cout << "\033[36m" << "X";
                    break;
                default:
                    std::cout << "\033[30m" << "X";
                    break;
            }
            if (++col == WIDTH) {
                std::cout << "\033[" << ++row << ";1H";
                col = 0;
            }
            ++k;
        }
        
        std::cout << std::endl;
    }
    
    void moveLeft() {
        if (isAllowed(current_stone_, current_stone_position_ - 1)) {
            current_stone_position_--;
        };
    }

    void moveRight() {
        if (isAllowed(current_stone_, current_stone_position_ + 1)) {
            current_stone_position_++;
        };
    }

    bool moveDown() {
        if (isAllowed(current_stone_, current_stone_position_ + WIDTH)) {
            current_stone_position_ += WIDTH;
            return true;
        };
        return false;
    }
    

    void rotateLeft() {
        Stone rotated = current_stone_.rotateLeft();
        if (isAllowed(rotated, current_stone_position_)) {
            current_stone_ = rotated;
        };
    }
    
    void rotateRight() {
        Stone rotated = current_stone_.rotateRight();
        if (isAllowed(rotated, current_stone_position_)) {
            current_stone_ = rotated;
        };
    }
    

    bool isAllowed(const Stone& stone, int position) {

        int idxStone = 0;
        const auto& stone_mask = stone.mask();
        for (auto i = 0; i < 4; ++i, position+=(WIDTH - 4)) {
            for (auto j = 0; j < 4; ++j, ++idxStone, ++position) {
                if (data_[position] != 0 && stone_mask[idxStone] != 0) {
                    return false;
                }
            }
        }
        return true;
    }
    
    bool newStone() {
        auto stone = stones[std::rand() % stones.size()];
        auto position = WIDTH / 2 - 2;
        if (isAllowed(stone, position)) {
            current_stone_ = stone;
            current_stone_position_ = position;
            return  true;
        }
        return false;
    }
    
    void materializeCurrentStone() {
        auto position = current_stone_position_;
        int idxStone = 0;
        for (auto i = 0; i < 4; ++i, position+=(WIDTH - 4)) {
            for (auto j = 0; j < 4; ++j, ++idxStone, ++position) {
                auto current = current_stone_.mask()[idxStone];
                if (current != 0) {
                    data_[position] = current_stone_.mask()[idxStone];
                }
            }
        }
    }

    int completeRows() {
        auto completedRows = 0;
        for (auto row = HEIGHT - 3; row > 0; --row) {
            if (isRowComplete(row)) {
                ++completedRows;
                clearRow(row);
                display();
                collapseRows(row);
                display();
            }
        }
        return completedRows;
    }

    bool isRowComplete(int row) {
        for (auto col = 0; col < WIDTH; ++col) {
            auto current = data_[row*WIDTH+col];
            if (current == 0) {
                return false;
            }
        }
        return true;
    }
    
    void collapseRows(int row) {
        for (auto idx = row * (WIDTH + 1); idx > WIDTH; --idx) {
            if (data_[idx] != 2) {
                data_[idx] = data_[idx - WIDTH];
            }
        }
    }
    
    void clearRow(int row) {
        for (auto col = 0; col < WIDTH; ++col) {
            auto current = data_[row*WIDTH+col];
            if (current != 2) {
                data_[row*WIDTH+col] = 0;
            }
        }
    }
private:
    std::array<uint8_t, WIDTH * HEIGHT> data_ {};
    Stone current_stone_;
    int current_stone_position_;
    
    inline int colFromStone(int k) {
        int stone_col {current_stone_position_ % WIDTH};
        int col {k % WIDTH};
        return col - stone_col;
    }
    
    inline int rowFromStone(int k) {
        int stone_row {current_stone_position_ / WIDTH};
        int row {k / WIDTH};
        return row - stone_row;
    }

};

enum class Command {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_DOWN,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    QUIT
    
};

class CommandReader {
    
public:
    
    CommandReader() {
    }
    
    std::experimental::optional<Command> nextCommand() {
        std::lock_guard<std::mutex> lock(mu);
        
        if (commands_.empty()) {
            return std::experimental::nullopt;
        } else {
            auto result {commands_.front()};
            commands_.pop();
            return result;
        }
    }
    
    ~CommandReader() {
        keyboard_reader_.join();
        system("stty -raw");
    }
    
private:
    
    std::queue<Command> commands_ {};
    std::mutex mu {};
    std::thread keyboard_reader_{&CommandReader::run, this};
    
    void run() {
        system("stty raw");
        
        int state = 0;
        
        while (true) {
            int c {getchar()};
            
            switch (state) {
                case 0:
                    switch (c) {
                        case 27:
                            state = 1;
                            break;
                        case 3:   // "^c"
                        case 97: // "a"
                            enqueue(Command::ROTATE_LEFT);
                            break;
                        case 115: // "s"
                            enqueue(Command::ROTATE_RIGHT);
                            break;
                        case 113: // "q" for Quit
                            enqueue(Command::QUIT);
                            return;
                    }
                    break;
                case 1:
                    if (c == 91) {
                        state = 2;
                    } else {
                        state = 0;
                    }
                    break;
                case 2:
                    switch (c) {
                        case 66:
                            enqueue(Command::MOVE_DOWN);
                            break;
                        case 67:
                            enqueue(Command::MOVE_RIGHT);
                            break;
                        case 68:
                            enqueue(Command::MOVE_LEFT);
                            break;
                        default:
                            break;
                    }
                    state = 0;
            }
        }
    }
    
    void enqueue(const Command cmd) {
        std::lock_guard<std::mutex> lock{mu};
        commands_.push(cmd);
    }
    
};

class Game {
    
public:
    
    void run() {
        
        int dropCounter = 0;
        bool lastDropSuccess = true;
        while (true) {
            auto cmd {cmdReader.nextCommand()};
            if (cmd) {
                switch (*cmd) {
                    case Command::QUIT:
                        return;
                    case Command::MOVE_LEFT:
                        playground.moveLeft();
                        break;
                    case Command::MOVE_RIGHT:
                        playground.moveRight();
                        break;
                    case Command::MOVE_DOWN:
                        playground.moveDown();
                        break;
                    case Command::ROTATE_LEFT:
                        playground.rotateLeft();
                        break;
                    case Command::ROTATE_RIGHT:
                        playground.rotateRight();
                        break;
                }
            }

            if (++dropCounter == 10) {
                bool success = playground.moveDown();
                if (!success && !lastDropSuccess) {
                    playground.materializeCurrentStone();
                    int completed_rows = playground.completeRows();
                    if (!playground.newStone()) {
                        std::cout << "Game over" << std::endl;
                        return;
                    }
                }
                lastDropSuccess = success;
                dropCounter = 0;
            }
            
            playground.display();
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            
        }
        
    }
    
private:
    
    Playground<18,20> playground {};
    CommandReader cmdReader {};
};


int main(int argc, const char * argv[]) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    Game game{};
    game.run();
    return 0;
}
