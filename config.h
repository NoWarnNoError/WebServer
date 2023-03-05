class Config {
   private:
    const char* PORT;
    const int BUFFER_SIZE;
    const int EVENTS_SIZE;

   public:
    Config();
    ~Config();

    const char* getPORT() const;
    const int getBUFFER_SIZE() const;
    const int getEVENTS_SIZE() const;
};
