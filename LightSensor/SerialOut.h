// Serial output class

class SerialOut : public Print
{
public:
  SerialOut (byte tx_pin);
  size_t write (uint8_t);
  void intKick ();

private:
  const byte tx_pin;
  volatile byte state;
  volatile byte buffer;
  volatile byte count;
};
