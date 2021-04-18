#include "game_initialization.h"

static void initialize_grid(Grid *grid, int height, int width){
    grid->height = height;
    grid->width = width;
    grid->grid = malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        *(grid->grid+i) = malloc(width * sizeof(char));
        memset((grid->grid[i]), 95, width * sizeof(char));
    }
}

static void set_inventory(Inventory *inventory, short int artillery, short int bomb, short int tactical, short int simple_missile){
    inventory->artillery = artillery;
    inventory->bomb = bomb;
    inventory->tactical = tactical;
    inventory->simple_missile = simple_missile;
}

static void initialize_inventory(Inventory *inventory, Difficulty difficulty){
    switch(difficulty){
        case EASY:
            set_inventory(inventory, 10, 10, 10, 10);
            break;
        case MEDIUM:
            set_inventory(inventory, 3, 5, 5, 10);
            break;
        case HARD:
            set_inventory(inventory, 1, 2, 4, 15);
            break;
        default:
            break;
    }
}

static void initialize_fleet(Boat *fleet){
    for(int i = 0; i < 5; i++){
        fleet[i].size = i < 2 ? i + 2 : i + 1;
        fleet[i].orientation = rand()%2;
    }
}

static int is_occupied(Grid *grid, Boat boat, short int line, short int column){
    int start_line = line, start_column = column;
    while((boat.orientation == VERTICAL && line - start_line < boat.size) || (boat.orientation == HORIZONTAL && (column - start_column < boat.size))){
        if(grid->grid[line][column] == 'B'){
            return 1;
        }
        line += (boat.orientation == VERTICAL) ? 1 : 0;
        column += (boat.orientation == HORIZONTAL) ? 1 : 0;
    }
    return 0;
}

static void set_boat(Grid *grid, Boat boat){
    for(int i = 0; i < boat.size; i++) {
        grid->grid[boat.position[0]][boat.position[1]] = 'B';
        boat.position[0] += (boat.orientation == VERTICAL) ? 1 : 0;
        boat.position[1] += (boat.orientation == HORIZONTAL) ? 1 : 0;
    }
}

static void set_fleet(Grid *grid, Boat *fleet){
    initialize_fleet(fleet);
    short int line, column;
    for(int i = 4; i >=0 ; i--){
        do{
            line = rand()%(grid->height - (fleet[i].size * (fleet[i].orientation == VERTICAL)));
            column = rand()%(grid->width - (fleet[i].size * (fleet[i].orientation == HORIZONTAL)));
        }while(is_occupied(grid, fleet[i], line, column));
        fleet[i].position[0] = line;
        fleet[i].position[1] = column;
        set_boat(grid, fleet[i]);
    }
}

static void set_difficulty(Difficulty *difficulty){
    int choice = 0;
    printf("Chose difficulty :\n  1 : Easy\n  2 : Medium\n  3 : Hard\n");
    input_choice(&choice, 1, 3);
    *difficulty = choice - 1;
}

static void set_gamemode(Mode *gamemode){
    int choice = 0;
    printf("Chose gamemode:\n  1 : Classic\n  2 : Blind\n  3 : Active\n");
    input_choice(&choice, 1, 3);
    *gamemode = choice - 1;
}

static void new_game(Grid *grid, Inventory *inventory, Difficulty *difficulty, Mode *gamemode, Boat *fleet){
    set_difficulty(difficulty);
    set_gamemode(gamemode);
    initialize_inventory(inventory, *difficulty);
    set_fleet(grid, fleet);
}

static void load(Grid *grid, Inventory *inventory, Difficulty *difficulty, Mode *gamemode, Boat *fleet){
    char *line = malloc(12 * sizeof(char));
    FILE *save = fopen("save.txt", "r");
    if(save){
        fscanf(save ,"%d\n%d\n%hu %hu %hu %hu\n", difficulty, gamemode, &(inventory->artillery), &(inventory->bomb), &(inventory->tactical), &(inventory->simple_missile));
        for(int i = 0;(fgets(line, 12 * sizeof(char), save)) != NULL; i++){
            strcpy(grid->grid[i], line);
        }
    }else{
        printf("No save file found, a new game will start.\n");
        new_game(grid, inventory, difficulty, gamemode, fleet);
    }
    free(line);
    fclose(save);
}

void initialization(Grid *grid, short int height, short int width, Inventory *inventory, Difficulty *difficulty, Mode *gamemode, Boat *fleet){
    int choice = 0;
    initialize_grid(grid, height, width);
    printf("Welcome to Naval Battle : enhanced edition\n");
    printf("Do you want to :\n  1 : Start a new game\n  2 : Resume the previous game\n  3 : Exit game\n");
    input_choice(&choice, 1, 3);
    switch(choice){
        case 1:
            new_game(grid, inventory, difficulty, gamemode, fleet);
            break;
        case 2:
            load(grid, inventory, difficulty, gamemode, fleet);
            break;
        default:
            exit(0);
    }
}