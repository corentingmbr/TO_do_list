#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>

typedef struct Node {
    SDL_Rect rect;
    char text[21];  // Le texte à afficher dans le rectangle
    int text_length; // La longueur du texte
    struct Node* next;
} Node;

Node* createNode(int x, int y, int w, int h) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->rect.x = x;
    newNode->rect.y = y;
    newNode->rect.w = w;
    newNode->rect.h = h;
    newNode->text[0] = '\0';  // Initialise le texte à une chaîne vide
    newNode->text_length = 0; // Initialise la longueur du texte à 0
    newNode->next = NULL;
    return newNode;
}
Node* loadData() {
    FILE* file = fopen("todo_list.csv", "r");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier\n");
        return NULL;
    }
    Node* rectangles = NULL;
    while (!feof(file)) {
        Node* newRect = (Node*)malloc(sizeof(Node));
        fscanf(file, "%d,%d,%d,%d,%[^\n]\n", &(newRect->rect.x), &(newRect->rect.y), &(newRect->rect.w), &(newRect->rect.h), newRect->text);
        newRect->text_length = strlen(newRect->text);
        newRect->next = rectangles;
        rectangles = newRect;
    }
    fclose(file);
    return rectangles;
}


void saveData(Node* rectangles) {
    FILE* file = fopen("todo_list.csv", "w");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier\n");
        return;
    }
    Node* temp = rectangles;
    while (temp != NULL) {
        fprintf(file, "%d,%d,%d,%d,%s\n", temp->rect.x, temp->rect.y, temp->rect.w, temp->rect.h, temp->text);
        temp = temp->next;
    }
    fclose(file);
}


void drawRects(SDL_Renderer* renderer, Node* node, TTF_Font* font, SDL_Color color) {
    Node* temp = node;
    while (temp != NULL) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Noir
        SDL_RenderFillRect(renderer, &(temp->rect));

        if (temp->text_length > 0) {
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, temp->text, color);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {temp->rect.x, temp->rect.y, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

            // Libérer la surface et la texture après les avoir dessinées
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }

        temp = temp->next;
    }
}



Node* deleteNode(Node* rectangles, Node* toDelete) {
    if (rectangles == toDelete) {
        Node* newHead = rectangles->next;
        free(rectangles);
        return newHead;
    }
    Node* temp = rectangles;
    while (temp != NULL && temp->next != toDelete) {
        temp = temp->next;
    }
    if (temp != NULL) {
        Node* toDeleteNext = toDelete->next;
        free(toDelete);
        temp->next = toDeleteNext;
    }
    return rectangles;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Todo List", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    TTF_Font* font = TTF_OpenFont("C:\\Users\\coren\\OneDrive\\Documents\\Cours\\Programmation\\Fichiers_C\\Todo_List\\Lora-VariableFont_wght.ttf", 24);
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color color = {255, 255, 255, 255}; // Blanc

    SDL_Surface* surface1 = TTF_RenderText_Solid(font, "To do", color);
    SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
    SDL_Rect rect1 = {100, 50, surface1->w, surface1->h};


    SDL_Surface* surface2 = TTF_RenderText_Solid(font, "Progress", color);
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    SDL_Rect rect2 = {350, 50, surface2->w, surface2->h};

    SDL_Surface* surface3 = TTF_RenderText_Solid(font, "Done", color);
    SDL_Texture* texture3 = SDL_CreateTextureFromSurface(renderer, surface3);
    SDL_Rect rect3 = {650, 50, surface3->w, surface3->h};

    SDL_Surface* surface4 = TTF_RenderText_Solid(font, "Add", color);
    SDL_Texture* texture4 = SDL_CreateTextureFromSurface(renderer, surface4);
    SDL_Rect rect4 = {720 - surface4->w / 2, 550 - surface4->h / 2, surface4->w, surface4->h};

    SDL_Surface* surface5 = TTF_RenderText_Solid(font, "(Right click to unselect task)", color);
    SDL_Texture* texture5 = SDL_CreateTextureFromSurface(renderer, surface5);
    SDL_Rect rect5 = {0, 600 - surface5->h, surface5->w, surface5->h};

    SDL_Surface* surface6 = TTF_RenderText_Solid(font, "Delete", color);
    SDL_Texture* texture6 = SDL_CreateTextureFromSurface(renderer, surface6);
    SDL_Rect rect6 = {120 - surface6->w / 2, 550 - surface6->h / 2, surface6->w, surface6->h};


    // Dessiner le bouton "Add"
    SDL_Rect button = {675, 525, 100, 50};
    // Dessiner le bouton "Delete"
    SDL_Rect deleteButton = {75, 525, 100, 50};


    SDL_Event event;
    int running = 1;
    Node* rectangles = loadData();;
    Node* dragging = NULL;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (x >= button.x && x <= button.x + button.w && y >= button.y && y <= button.y + button.h) {
                        Node* newRect = createNode(375, 275, 260, 30);
                        newRect->next = rectangles;
                        rectangles = newRect;
                    }
                    if (x >= deleteButton.x && x <= deleteButton.x + deleteButton.w && y >= deleteButton.y && y <= deleteButton.y + deleteButton.h) {
                        Node* temp = rectangles;
                        while (temp != NULL) {
                            if (x >= temp->rect.x && x <= temp->rect.x + temp->rect.w && y >= temp->rect.y && y <= temp->rect.y + temp->rect.h) {
                                rectangles = deleteNode(rectangles, temp);
                                break;
                            }
                            temp = temp->next;
                        }
                    }

                    Node* temp = rectangles;
                    while (temp != NULL) {
                        if (x >= temp->rect.x && x <= temp->rect.x + temp->rect.w && y >= temp->rect.y && y <= temp->rect.y + temp->rect.h) {
                            dragging = temp;
                            SDL_StartTextInput();  // Commence la saisie de texte
                            break;
                        }
                        temp = temp->next;
                    }
                }else if (event.button.button == SDL_BUTTON_RIGHT){
                    dragging = NULL;
                    SDL_StopTextInput();  // Arrête la saisie de texte
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {


            }
            if (event.type == SDL_MOUSEMOTION && dragging) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                dragging->rect.x = x - dragging->rect.w / 2;
                dragging->rect.y = y - dragging->rect.h / 2;
            }

            if (event.type == SDL_TEXTINPUT && dragging) {
                if (dragging->text_length < 255) {
                    dragging->text[dragging->text_length] = event.text.text[0];
                    dragging->text_length++;
                    dragging->text[dragging->text_length] = '\0';
                }
            }

        }


        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Dessiner les colonnes de couleur
        SDL_Rect column1 = {0, 0, 266, 600};
        SDL_SetRenderDrawColor(renderer, 120, 76, 139, 255); // Violet
        SDL_RenderFillRect(renderer, &column1);

        SDL_Rect column2 = {267, 0, 266, 600};
        SDL_SetRenderDrawColor(renderer, 220, 185, 103, 255); // Jaune
        SDL_RenderFillRect(renderer, &column2);

        SDL_Rect column3 = {534, 0, 266, 600};
        SDL_SetRenderDrawColor(renderer, 43, 94, 142, 255); // Bleu
        SDL_RenderFillRect(renderer, &column3);

        // Dessiner les rectangles noirs
        drawRects(renderer, rectangles, font, color);

        // Dessiner le bouton "Add"
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Noir
        SDL_RenderFillRect(renderer, &button);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Noir
        SDL_RenderFillRect(renderer, &deleteButton);
        SDL_RenderCopy(renderer, texture6, NULL, &rect6);

        // Dessiner le texte par-dessus les colonnes
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderCopy(renderer, texture1, NULL, &rect1);
        SDL_RenderCopy(renderer, texture2, NULL, &rect2);
        SDL_RenderCopy(renderer, texture3, NULL, &rect3);
        SDL_RenderCopy(renderer, texture4, NULL, &rect4);
        SDL_RenderCopy(renderer, texture5, NULL, &rect5);

        SDL_RenderPresent(renderer);
    }

    SDL_FreeSurface(surface1);
    SDL_DestroyTexture(texture1);
    SDL_FreeSurface(surface2);
    SDL_DestroyTexture(texture2);
    SDL_FreeSurface(surface3);
    SDL_DestroyTexture(texture3);
    SDL_FreeSurface(surface4);
    SDL_DestroyTexture(texture4);
    SDL_FreeSurface(surface5);
    SDL_DestroyTexture(texture5);
    SDL_FreeSurface(surface6);
    SDL_DestroyTexture(texture6);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    saveData(rectangles);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
