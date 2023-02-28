class Config {
   private:
    const char* PORT;

   public:
    Config();
    ~Config();

    const char* getPORT() const;
};
