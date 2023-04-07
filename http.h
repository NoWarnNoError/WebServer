#pragma once

class HTTP {
   private:
    int state;

   public:
    HTTP();
    ~HTTP();

    int get_state();
    void set_state(int __state);
};
