class Config {
   private:
    const char* PORT;
    const int BUFFER_SIZE;
    const int EVENTS_SIZE;
    const int THREADS_MAX;
    const int REQUESTS_MAX;

   public:
    Config();
    ~Config();

    const char* getPORT() const;
    const int getBUFFER_SIZE() const;
    const int getEVENTS_SIZE() const;
    const int getTHREADS_MAX() const;
    const int getREQUESTS_MAX() const;
};
