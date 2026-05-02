#include "rbms.h"

void gm6020::set_default_param(){
    for(int id=0;id<_motor_num;id++){
        if (_motor_type[id]) { // トルク//動きはする
            _pid_gains[id]._kp = 45.0f; _pid_gains[id]._ki = 35.0f; _pid_gains[id]._kd = 0.0f;
            _pid_gains[id]._kp_p = 5.0f; _pid_gains[id]._ki_p = 0.0f; _pid_gains[id]._kd_p = 0.15f;
            _motor_max[id] = 16384;
        } else { // 速度(未実装→別クラスのほうがいい？)
            //_kp = 15.0f; _ki = 12.0f; _kd = 0.0f;//最初から角速度指定なので初期値(0)のまま
            _pid_gains[id]._kp_p = 4.5f; _pid_gains[id]._ki_p = 0.0f; _pid_gains[id]._kd_p = 0.25f;
            _motor_max[id] = 25000;
        }
    }
}

void gm6020::set_gear_ratio(int id, float gear_raito){
    if (id < 0 || id >= _motor_num) return;
    _data_mutex.lock();
    _gear_ratio[id] = gear_raito==0 ? 1 : gear_raito;
    _data_mutex.unlock();
}

int gm6020::rbms_send() {
    _tx_msg_low.id = 0x1fe; _tx_msg_low.len = 8;
    _tx_msg_high.id = 0x2fe; _tx_msg_high.len = 8;
    _data_mutex.lock();
    for(int i = 0; i < _motor_num; i++) {
        int val = _output_torques[i];
        if (i < 4) {
            _tx_msg_low.data[i*2] = (char)(val >> 8);
            _tx_msg_low.data[i*2+1] = (char)(val & 0xFF);
        } else {
            _tx_msg_high.data[(i-4)*2] = (char)(val >> 8);
            _tx_msg_high.data[(i-4)*2+1] = (char)(val & 0xFF);
        }
    }
    _data_mutex.unlock();
    return (_can.write(_tx_msg_low) && (_motor_num > 4 ? _can.write(_tx_msg_high) : true)) ? 1 : -1;
}

bool gm6020::handle_message(const CANMessage &msg){
    int id_idx = msg.id - 0x205;
    if (id_idx >= 0 && id_idx < _motor_num) {
        _data_mutex.lock();
        _msg_buffer[id_idx] = msg;
        _new_data_mask |= (1 << id_idx);
        _data_mutex.unlock();
        _event_flags.set(0x01); 
        return true;
    }
    return false;
}
