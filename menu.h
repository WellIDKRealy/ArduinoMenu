enum MenuItemType {
  MenuItemType,
  MenuProgramType,
  MenuType,
};

typedef union MenuItemU {
  struct MenuItem* menu_item;
  struct MenuProgram* menu_program;
  struct Menu* menu;
} menu_item_union_t;

typedef struct MenuItem {
  enum MenuItemType type;
  char* name;
} menu_item_t;

typedef struct MenuProgram {
  enum MenuItemType type;
  char* name;

  void (*program_f)(void);
} menu_program_t;

typedef struct Menu {
  enum MenuItemType type;
  char* name;

  uint8_t origin;
  uint8_t selected;

  uint8_t size;
  menu_item_union_t items[];
} menu_t;


typedef struct MenuStack {
  struct MenuStack* back;
  menu_t* menu;
} menu_stack_t;
