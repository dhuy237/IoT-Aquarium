#include <stdint.h>
#include <sdk.h>
class sche2 {
   public:
      sdk _sdk;
      int *time_storage;
      int _size;
      int _capa;
      int diw;
      sche2() {
        this->_size = 0;
        this->_capa = 10;
	diw = 0;
        this->time_storage = new int[10];
	_sdk.SDcheck();
      }
      ~sche2() {
        delete[] time_storage;
      }
      int enCapa() {
          if(this->_size >= this->_capa) {
            this->_capa = 2 * this->_capa;
            int *temp1 = new int[this->_capa];
            memcpy(temp1,time_storage, this->_size*sizeof(int));
            delete[] time_storage;
            time_storage = temp1;
          }
      }

      void pri() {
	 for(int i=0;i<_size;i++)
		Serial.println(time_storage[i]);
      }

      void insert(int _time, int _cmd, int _diw) {
	  if(this->diw != _diw)
		return;
          this->enCapa();
	  int i;
	  for(i=0;i<_size;i++) 
		if(_time * 10 + _cmd  > time_storage[i])
			break;
	  if(i > 0 && time_storage[i - 1] == _time * 10 + _cmd)
		return;
	  memmove(time_storage + i + 1, time_storage + i, sizeof(int) * (_size - i));
	  time_storage[i] = _time * 10 + _cmd;
	  _size++;
	  pri();
      }

      void del(int _time, int _cmd, int _diw) {
	  for(int i=0;i<_size;i++) 
		if(time_storage[i] == _time * 10 + _cmd) {
			memmove(time_storage + i, time_storage + i + 1, _size - i - 1);
			_size--;
                }
      }

      int pop(String Current_time) {
	    int _time = (Current_time[6] - '0') * 1000 + (Current_time[7] - '0') * 100 + (Current_time[9] - '0') * 10 + (Current_time[10] - '0');
	    if(_size > 0 && _time > time_storage[_size - 1] / 10)
		this->_size--;
            if(_size > 0 && _time == time_storage[_size - 1] / 10) {
                this->_size--;
                return time_storage[_size] % 10;
            }
            return 0;
      }

      /*void upda(int _diw) {
            this->diw = _diw;
	    delete [] time_storage;
      	    _size = 0;
            _capa = 10;
	    time_storage = new int[10];
	    for(int i=0;i<23;i++)
		for(int j=0;j<59;j++) {
			String filename = String(diw) + String(i) + String(j);
			int _time = i * 100 + j;
			String temp = _sdk.SDprint(filename);
			for(int k=0;k<temp.length();k++)
				insert(_time, temp[k] - '0', diw);
		}
			
      }*/

      void getdata(char * topic, byte *content, unsigned int length_of_content) {
	 Serial.println("Recieved");
  	 String rec;
  	 rec = String((char*)content);
   	 rec.remove(length_of_content);
  	 int _time = 0;
	 int _diw = rec[0] - '0';
  	 for(int i=1;i<5;i++)
  	 	_time = _time * 10 + (int) (rec[i] - '0');
  	 int _cmd = (int) (rec[6] - '0');
  	 if(rec[5] == '1') {
		insert(_time, _cmd, _diw);
		_sdk.SDcheck();
		_sdk.SDwrite(rec.substring(0,5), (String) (rec[6]));
		_sdk.SDprint(rec.substring(0,5));
	 }
	 else {
		del(_time, _cmd, _diw);
		_sdk.SDdelete(rec.substring(0,5), (String) (rec[6]));
	 }
      }
};