#ifndef __BluetoothLE_h
#define __BluetoothLE_h

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <functional>
#include "Serializable.h"

extern "C" {

// Note well: we're trying really hard to only use soft device headers here and even then never in the public interface.

#include "ble_gap.h"
#include "ble.h"
#include "ble_advdata.h"
#include "ble_srv_common.h"
#include "nrf_sdm.h"
#include "ble_gatts.h"
//#include "app_timer.h"
#include "ble_gatts.h"
#include "ble_gatt.h"
#include "nrf.h"
#include "nrf_soc.h"

}

using std::function;
using std::vector;
using std::string;
using std::exception;

// TOOD replace vector with a fixed, in place array of size capacity.
template<typename T> class tuplev : public vector<T> {
  public:
    tuplev() {}
};

template<typename T, uint8_t capacity> class fixed_tuple : public tuplev<T> {
  public:

    fixed_tuple<T, capacity>() : tuplev<T>() {this->reserve(capacity);}

};

extern "C" {
void ble_error_handler (string msg, uint32_t line_num, const char * p_file_name);
extern void softdevice_assertion_handler(uint32_t pc, uint16_t line_number, const uint8_t * p_file_name);
void SWI1_IRQHandler(void);
}

namespace BLEpp {

#ifdef __EXCEPTIONS

    class ble_exception : public exception {
      public:
        string _message;
        ble_exception(string message, string file = "<unknown>", int line = 0) : _message(message) {}
        ~ble_exception() throw() {}

        virtual char const *what() const throw() {
            return _message.c_str();
        }
    };


#define BLE_CALL(function, args) do {uint32_t result = function args; if (result != NRF_SUCCESS) throw ble_exception(# function, __FILE__, __LINE__); } while(0)
#define BLE_THROW_IF(result, message) do {if (result != NRF_SUCCESS) throw ble_exception(message, __FILE__, __LINE__); } while(0)
#define BLE_THROW(message) throw ble_exception(message, __FILE__, __LINE__)


#else /* __EXCEPTIONS */

#define BLE_CALL(function, args) do {volatile uint32_t result = function args; if (result != NRF_SUCCESS) {string ble_error_message(# function ); ble_error_handler(ble_error_message, __LINE__, __FILE__); } } while(0)
#define BLE_THROW_IF(result, message) do {if (result != NRF_SUCCESS) { string ble_error_message(message); ble_error_handler(ble_error_message, __LINE__, __FILE__); } } while(0)
#define BLE_THROW(message) do { string ble_error_message(message); ble_error_handler(ble_error_message, __LINE__, __FILE__); } while(0)

#endif /* __EXCEPTIONS */

    class Service;
    class BLEStack;

    /// UUID ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    class UUID { // abstracts ble_uid_t

      protected:
        const char*           _full; //* proprietary UID strung.  null for standard UIDs.
        uint16_t              _uuid; //*< 16-bit UUID value or octets 12-13 of 128-bit UUID.
        uint8_t               _type; //*< UUID type, see @ref BLE_UUID_TYPES.

      public:
        operator ble_uuid_t() {
            ble_uuid_t ret;
            ret.uuid = _uuid;
            ret.type = _type;
            return ret;
        }

        UUID() : _uuid(0xdead), _type(BLE_UUID_TYPE_UNKNOWN) {};
        UUID(const char* fullUid);
        UUID(uint16_t uuid) : _full(0), _uuid(uuid), _type(BLE_UUID_TYPE_BLE) { }
        UUID(UUID& parent, uint16_t uidValue) : _full(0), _uuid(uidValue), _type(parent.init()) {
        }
        UUID(const UUID& rhs) :_full(rhs._full), _uuid(rhs._uuid), _type(rhs._type) {}
        UUID(const ble_uuid_t& u) : _full(0), _uuid(u.uuid), _type(u.type) {} // FIXME NRFAPI

        uint16_t getUuid() const {
            return _uuid;
        }

        uint8_t getType() {
            init();
            return _type;
        }

        uint16_t init();

    };

    /// Characteristics ////////////////////////////////////////////////////////////////////////////////////////////////

    struct CharacteristicValue {
        uint16_t length;
        const uint8_t* data; // TODO use refcount to manage lifetime?  current assumes it to be valid for as long as needed to send to stack.
        CharacteristicValue(uint16_t _length, const uint8_t *const _data) : length(_length), data(_data) {}
        CharacteristicValue() : length(0), data(0) {}
        bool operator==(const CharacteristicValue& rhs) {
            return rhs.data == data && rhs.length == length;
        }
        bool operator!=(const CharacteristicValue& rhs) {
            return !operator==(rhs);
        }
        CharacteristicValue operator=(const CharacteristicValue& rhs) {
            data = rhs.data;
            length = rhs.length;
            return *this;
        }
    };

    struct CharacteristicInit {
        ble_gatts_attr_t          attr_char_value;
        ble_gatts_char_pf_t       presentation_format;
        ble_gatts_char_md_t       char_md;
        ble_gatts_attr_md_t       cccd_md;
        ble_gatts_attr_md_t       sccd_md;
        ble_gatts_attr_md_t       attr_md;
        ble_gatts_attr_md_t       user_desc_metadata_md;

        CharacteristicInit() : presentation_format({}), char_md({}), cccd_md({}), attr_md({}) {}
    };

    /** Non-template base class for Characteristics.  Saves on code size. */
    class CharacteristicBase {

      public:

      protected:

        UUID                      _uuid;   // 8
        string                    _name;  //4
//        ble_gatts_attr_t          _attr_char_value;
//        ble_gatts_char_pf_t       _presentation_format;
//        ble_gatts_char_md_t       _char_md;
//        ble_gatts_attr_md_t       _cccd_md;
//        ble_gatts_attr_md_t       _sccd_md;
//        ble_gatts_attr_md_t       _attr_md;
        ble_gap_conn_sec_mode_t   _readperm;  // 1
        ble_gap_conn_sec_mode_t   _writeperm; // 1
        ble_gatts_char_handles_t  _handles;   // 8

        Service*                  _service;  // 4

        bool                      _inited;
        bool                      _notifies;
        bool                      _writable;
        uint16_t                  _unit;   // 2
        uint32_t                  _updateIntervalMsecs; // 0 means don't update.  // 4
        //app_timer_id_t            _timer;  // 4

      public:

        CharacteristicBase();
        virtual ~CharacteristicBase() {}

        void init(Service* svc);

        CharacteristicBase& setWritable(bool writable) {
            _writable = writable;
            setWritePermission(1, 1);
            return *this;
        }

        void setupWritePermissions(CharacteristicInit& ci);

        CharacteristicBase& setNotifies(bool notifies) {
            _notifies = notifies;
            return *this;
        }

        /* Security Mode 0 Level 0: No access permissions at all (this level is not defined by the Bluetooth Core specification).\n
         * Security Mode 1 Level 1: No security is needed (aka open link).\n
         * Security Mode 1 Level 2: Encrypted link required, MITM protection not necessary.\n
         * Security Mode 1 Level 3: MITM protected encrypted link required.\n
         * Security Mode 2 Level 1: Signing or encryption required, MITM protection not necessary.\n
         * Security Mode 2 Level 2: MITM protected signing required, unless link is MITM protected encrypted.\n
        */
        CharacteristicBase&  setWritePermission(uint8_t securityMode, uint8_t securityLevel) {
            _writeperm.sm = securityMode;
            _writeperm.lv = securityLevel;
            return *this;
        }

        CharacteristicBase& setUUID(const UUID& uuid) {
            if (_inited) BLE_THROW("Already inited.");
            _uuid = uuid;
            return *this;
        }

        CharacteristicBase& setName(const string& name);

        CharacteristicBase& setUnit(uint16_t unit);

        const UUID & getUUID() {
            return _uuid;
        }

        uint16_t getValueHandle() {
            return _handles.value_handle;
        }

        CharacteristicBase& setUpdateIntervalMSecs(uint32_t msecs);

        virtual uint16_t getValueMaxLength() = 0;

        virtual CharacteristicValue getCharacteristicValue() = 0;
        virtual void setCharacteristicValue(const CharacteristicValue& value) = 0;
        virtual void read() = 0;
        virtual void written(uint16_t len, uint16_t offset, uint8_t* data) = 0;

      protected:

        virtual void configurePresentationFormat(ble_gatts_char_pf_t &) {}

        void notify();
    };



    template<typename T> inline  uint8_t ble_type() {
        return BLE_GATT_CPF_FORMAT_STRUCT;
    }
    template<> inline uint8_t ble_type<string>() {
        return BLE_GATT_CPF_FORMAT_UTF8S;
    }
    template<> inline uint8_t ble_type<uint8_t>() {
        return BLE_GATT_CPF_FORMAT_UINT8;
    }
    template<> inline uint8_t ble_type<uint16_t>() {
        return BLE_GATT_CPF_FORMAT_UINT16;
    }
    template<> inline uint8_t ble_type<uint32_t>() {
        return BLE_GATT_CPF_FORMAT_UINT32;
    }
    template<> inline uint8_t ble_type<int8_t>() {
        return BLE_GATT_CPF_FORMAT_SINT8;
    }
    template<> inline uint8_t ble_type<int16_t>() {
        return BLE_GATT_CPF_FORMAT_SINT16;
    }
    template<> inline uint8_t ble_type<int32_t>() {
        return BLE_GATT_CPF_FORMAT_SINT32;
    }
    template<> inline uint8_t ble_type<float>() {
        return BLE_GATT_CPF_FORMAT_FLOAT32;
    }
    template<> inline uint8_t ble_type<double>() {
        return BLE_GATT_CPF_FORMAT_FLOAT64;
    }
    template<> inline uint8_t ble_type<bool>() {
        return BLE_GATT_CPF_FORMAT_BOOLEAN;
    }

    template<class T>
    class Characteristic : public CharacteristicBase {

        friend class Service;

      public:
        typedef function<void(const T&)> callback_on_write_t;
        typedef function<T()> callback_on_read_t;

      protected:

        T                          _value;
        callback_on_write_t        _callbackOnWrite;
        callback_on_read_t         _callbackOnRead;

      public:
        Characteristic() {
        }
        virtual ~Characteristic() {}

        virtual uint16_t getValueLength() {
            return sizeof(T);
        }

        virtual uint16_t getValueMaxLength() {
            return sizeof(T);
        }

        const T& getValue() const {
            return _value;
        }

        void setValue(const T& value) {
            _value = value;

            notify();
        }

        Characteristic<T>& setUUID(const UUID& uuid) {
            CharacteristicBase::setUUID(uuid);
            return *this;
        }

        Characteristic<T>& setName(const string& name) {
            CharacteristicBase::setName(name);
            return *this;
        }

        Characteristic<T>& setUnit(uint16_t unit) {
            CharacteristicBase::setUnit(unit);
            return *this;
        }

        Characteristic<T>& setWritable(bool writable) {
            CharacteristicBase::setWritable(writable);
            return *this;
        }

        Characteristic<T>& onWrite(const callback_on_write_t& closure) {
            _callbackOnWrite = closure;
            return *this;
        }

        Characteristic<T>& onRead(const callback_on_read_t& closure) {
            _callbackOnRead = closure;
            return *this;
        }

        Characteristic<T>& setUpdateIntervalMSecs(uint32_t msecs) {
            CharacteristicBase::setUpdateIntervalMSecs(msecs);
            return *this;
        }

        operator T&() const {
            return _value;
        }

        Characteristic<T>& operator=(const T& val) {
            _value = val;

            notify();

            return *this;
        }

        Characteristic<T>& operator+=(const T& val) {
            _value += val;

            notify();

            return *this;
        }

        Characteristic<T>& operator-=(const T& val) {
            _value -= val;

            notify();

            return *this;
        }

        Characteristic<T>& operator*=(const T& val) {
            _value *= val;

            notify();

            return *this;
        }

        Characteristic<T>& operator/=(const T& val) {
            _value /= val;

            notify();

            return *this;
        }

        Characteristic<T>& setDefaultValue(const T& t) {
            if (_inited) BLE_THROW("Already inited.");
            _value = t;
            return *this;
        }

      protected:

        void written(uint16_t len, uint16_t offset, uint8_t* data) {
            CharacteristicValue value(len, data+offset);
            setCharacteristicValue(value);

            // only callback if there actually is one
            if (_callbackOnWrite)
                _callbackOnWrite(getValue());
        }

        void read() {
            T newValue = _callbackOnRead();
            if (newValue != _value) {
                operator=(newValue);
            }
        }


        virtual void configurePresentationFormat(ble_gatts_char_pf_t& presentation_format) {
            presentation_format.format = ble_type<T>();
            presentation_format.name_space = BLE_GATT_CPF_NAMESPACE_BTSIG;
            presentation_format.desc = BLE_GATT_CPF_NAMESPACE_DESCRIPTION_UNKNOWN;
            presentation_format.exponent = 1;
        }

      private:
    };


    template<typename T, typename E = void> class CharacteristicT : public Characteristic<T> {
      public:
        CharacteristicT()
        : Characteristic<T>() {
        }

        CharacteristicT& operator=(const T& val) {
            Characteristic<T>::operator=(val);
            return *this;
        }
        virtual CharacteristicValue getCharacteristicValue() = 0; // defined only in specializations.
        virtual void setCharacteristicValue(const CharacteristicValue& value) = 0; // defined only in specializations.
    };



    // this specialization handles all built-in arithmetic types (int, float, etc)
    template<typename T > class CharacteristicT<T, typename std::enable_if<std::is_base_of<ISerializable, T>::value >::type> : public Characteristic<T> {
       public:
        CharacteristicT& operator=(const T& val) {
            Characteristic<T>::operator=(val);
            return *this;
        }

        virtual CharacteristicValue getCharacteristicValue() {
            CharacteristicValue value;
            const T& t = this->getValue();
            Buffer b;
            b.allocate(t.getSerializedLength());
            t.serialize(b);
            b.release();
            return CharacteristicValue(b.length, b.data);
        }
        virtual void setCharacteristicValue(const CharacteristicValue& value) {
            T t;
            Buffer b(value.length, (uint8_t*)value.data);
            t.deserialize(b);
            this->setValue(t);
        }
    };

    // this specialization handles all types that implement ISerializable
    template<typename T > class CharacteristicT<T, typename std::enable_if<std::is_arithmetic<T>::value >::type> : public Characteristic<T> {
    public:
        CharacteristicT& operator=(const T& val) {
            Characteristic<T>::operator=(val);
            return *this;
        }

        virtual CharacteristicValue getCharacteristicValue() {
            // TODO: endianness.
            return CharacteristicValue(sizeof(T), (uint8_t*)&this->getValue());
        }
        virtual void setCharacteristicValue(const CharacteristicValue& value) {
            // TODO: endianness.
            this->setValue(*(T*)(value.data));
        }


    };

    template<> class CharacteristicT<string> : public Characteristic<string> {
      public:
        CharacteristicT& operator=(const string& val) {
            Characteristic<string>::operator=(val);
            return *this;
        }

        virtual uint16_t getValueMaxLength() {
            return 25;
        }

        virtual CharacteristicValue getCharacteristicValue() {
            return CharacteristicValue(getValue().length(), (uint8_t*)getValue().c_str());
        }
        virtual void setCharacteristicValue(const CharacteristicValue& value) {
            setValue(string((char*)value.data, value.length));
        }
    };

    template<> class CharacteristicT<CharacteristicValue> : public Characteristic<CharacteristicValue> {
    public:
        CharacteristicT& operator=(const CharacteristicValue& val) {
            Characteristic<CharacteristicValue>::operator=(val);
            return *this;
        }

        virtual uint16_t getValueMaxLength() {
            return 25; // TODO
        }

        virtual CharacteristicValue getCharacteristicValue() {
            return getValue();
        }
        virtual void setCharacteristicValue(const CharacteristicValue& value) {
            setValue(value);
        }
    };

//    template<> class Characteristic<char*> : public CharacteristicT<char*> {
//        virtual CharacteristicValue getCharacteristicValue() {
//            const char* val = getValue();
//
//            return CharacteristicValue(val == 0 ? 0 : strlen(val), val == 0 ? 0 : (uint8_t*)val);
//        }
//    };

    // TODO: more specializations.

    /// Services ///////////////////////////////////////////////////////////////////////////////////////////////////////

    class Service {
        friend class BLEStack;

      public:
        static const char* defaultServiceName; // "Generic Service"

        typedef tuplev<CharacteristicBase*> Characteristics_t;

      protected:

        BLEStack*          _stack;
        UUID               _uuid;
        string             _name;
        bool               _primary;
        uint16_t           _service_handle; // provided by stack.
        bool               _started;

      public:
        Service()
            :
              _name(),
              _primary(true),
              _service_handle(BLE_CONN_HANDLE_INVALID),
              _started(false)
        {


        }
        virtual ~Service() {
            // we don't currently delete our characteristics as we don't really support dynamic service destruction.
            // if we wanted to allow services to be removed at runtime, we would need to, amongst many other things,
            // keep track of whether we allocated the characteristic or whether it was passed into us.
        }

        Service& setName(const string& name) {
            _name = name;
            return *this;
        }

        Service& setUUID(const UUID& uuid) {
            if (_started) BLE_THROW("Already started.");

            _uuid = uuid;
            return *this;
        }

        Service& setIsPrimary(bool isPrimary) {
            if (_started) BLE_THROW("Already started.");

            _primary = isPrimary;
            return *this;
        }

        BLEStack* getStack() {
            return _stack;
        }

        UUID& getUUID() {
            return _uuid;
        }

        uint16_t getHandle() {
            return _service_handle;
        }

        /** Registers a new Characteristic with the given uid, name, and intialValue. Pass the type of the
        * characteristic in angle brackets before the parentheses for the method invocation:
        *
        * <pre>
        *      service.createCharacteristic<string>().setName("Owner Name").setDefaultValue("Bob Roberts");
        *      service.createCharacteristic<int>().setName("Owner Age").setDefaultValue(39);
        *      service.createCharacteristic<float>().setName("Yaw").setDefaultValue(0.0);
        * */
        template <typename T> Characteristic<T>& createCharacteristic() {
            Characteristic<T>* base = new CharacteristicT<T>();
            addCharacteristic(base);
            return *base;
        }

        // internal:

        virtual void start(BLEStack* stack);
        virtual void stop() {}

        virtual Service& addCharacteristic(CharacteristicBase* characteristic) = 0;

        virtual Characteristics_t& getCharacteristics() = 0;

        void on_ble_event(ble_evt_t * p_ble_evt);

        virtual void on_connect(uint16_t conn_handle, ble_gap_evt_connected_t& gap_evt);  // FIXME NRFAPI  friend??

        virtual void on_disconnect(uint16_t conn_handle, ble_gap_evt_disconnected_t& gap_evt);  // FIXME NRFAPI

        virtual void on_write(ble_gatts_evt_write_t& write_evt);  // FIXME NRFAPI

    };

    class GenericService : public Service {

      protected:

        static const uint8_t MAX_CHARACTERISTICS = 6;

        // by keeping the container in this subclass, we can in the future define services that more than MAX_CHARACTERISTICS characteristics.

        fixed_tuple<CharacteristicBase*, MAX_CHARACTERISTICS> _characteristics;

      public:

        GenericService() : Service() {}


        virtual Characteristics_t & getCharacteristics() {
            return _characteristics;
        }
        virtual GenericService& addCharacteristic(CharacteristicBase* characteristic) {
            if (_characteristics.size() == MAX_CHARACTERISTICS) {
                BLE_THROW("Too many characteristics.");
            }
            _characteristics.push_back(characteristic);

            return *this;
        }
    };

    class BatteryService : public GenericService {

      public:
        typedef function<uint8_t()> func_t;

      protected:
        CharacteristicT<uint8_t> _characteristic;
        func_t _func;
      public:
        BatteryService() {
            setUUID(UUID(BLE_UUID_BATTERY_SERVICE));
            setName("BatteryService");

            _characteristic.setUUID(UUID(BLE_UUID_BATTERY_LEVEL_CHAR))
                           .setName(string("battery level"))
                           .setDefaultValue(100);

            addCharacteristic(&_characteristic);
        }

        void setBatteryLevel(uint8_t batteryLevel){
            _characteristic = batteryLevel;
        }
        void setBatteryLevelHandler(func_t func) {
            _func = func;
        }
    };


    /// Stack //////////////////////////////////////////////////////////////////////////////////////////////////////////


    class BLEStack {
        public:
        virtual bool connected() = 0;
        virtual uint16_t getConnectionHandle() = 0;
        // TODO more here.
    };
    class Nrf51822BluetoothStack : public BLEStack {
        friend void SWI2_IRQHandler();   // ble stack events.
        friend void ::SWI1_IRQHandler(); // radio notification
      public:
        typedef function<void(uint16_t conn_handle)>   callback_connected_t;
        typedef function<void(uint16_t conn_handle)>   callback_disconnected_t;
        typedef function<void(bool radio_active)>   callback_radio_t;


        static Nrf51822BluetoothStack * _stack;

    public:
        static const uint8_t MAX_SERVICE_COUNT = 5;

        static const uint16_t                  defaultAppearance = BLE_APPEARANCE_GENERIC_DISPLAY;
        static const nrf_clock_lfclksrc_t      defaultClockSource = NRF_CLOCK_LFCLKSRC_XTAL_75_PPM;
        static const uint8_t                   defaultMtu = BLE_L2CAP_MTU_DEF;
        static const uint16_t                  defaultMinConnectionInterval_1_25_ms = 20;
        static const uint16_t                  defaultMaxConnectionInterval_1_25_ms = 20;
        static const uint16_t                  defaultSlaveLatencyCount = 0;
        static const uint16_t                  defaultConnectionSupervisionTimeout_10_ms = 400;
        static const uint16_t                  defaultAdvertisingInterval_0_625_ms = 40;
        static const uint16_t                  defaultAdvertisingTimeout_seconds = 0;
        static const int8_t                    defaultTxPowerLevel = 4;

    protected:


        string                                      _device_name; // 4
        uint16_t                                    _appearance;
        fixed_tuple<Service*, MAX_SERVICE_COUNT>    _services;  // 32

        nrf_clock_lfclksrc_t                        _clock_source; //4
        uint8_t                                     _mtu_size;
        int8_t                                      _tx_power_level;
        uint8_t *                                   _evt_buffer;
        uint32_t                                    _evt_buffer_size;
        ble_gap_conn_sec_mode_t                     _sec_mode;  //1
        //ble_gap_sec_params_t                        _sec_params; //6
        //ble_gap_adv_params_t                        _adv_params; // 20
        uint16_t                                    _interval;
        uint16_t                                    _timeout;
        ble_gap_conn_params_t                       _gap_conn_params; // 8

        bool                                        _inited;
        bool                                        _started;
        bool                                        _advertising;


        callback_connected_t                        _callback_connected;  // 16
        callback_disconnected_t                     _callback_disconnected;  // 16



    public:
        callback_radio_t _callback_radio;  // 16
		volatile uint8_t _radio_notify; // 0 = no notification (radio off), 1 = notify radio on, 2 = no notification (radio on), 3 = notify radio off.
		Nrf51822BluetoothStack();
		ble_gap_addr_t _peer_addr;
		bool _irk_match;
		uint8_t _irk_match_idx;
		ble_gap_sec_params_t sec_params;
		uint16_t  _conn_handle;
        virtual ~Nrf51822BluetoothStack();

        void process_event();

        Nrf51822BluetoothStack& init();

        Nrf51822BluetoothStack& rename();

        Nrf51822BluetoothStack& start();

        Nrf51822BluetoothStack& shutdown();

        Nrf51822BluetoothStack& setAppearance(uint16_t appearance) {
            if (_inited) BLE_THROW("Already initialized.");
            _appearance = appearance;
            return *this;
        }

        Nrf51822BluetoothStack& setDeviceName(const string& deviceName) {
            //if (_inited) BLE_THROW("Already initialized.");
            _device_name = deviceName;
            return *this;
        }

        Nrf51822BluetoothStack& setClockSource(nrf_clock_lfclksrc_t clockSource) {
            if (_inited) BLE_THROW("Already initialized.");
            _clock_source = clockSource;
            return *this;
        }

        Nrf51822BluetoothStack& setAdvertisingInterval(uint16_t advertisingInterval){
            // TODO: stop advertising?
            _interval = advertisingInterval;
            return *this;
        }

        Nrf51822BluetoothStack& setAdvertisingTimeoutSeconds(uint16_t advertisingTimeoutSeconds) {
            // TODO: stop advertising?
            _timeout = advertisingTimeoutSeconds;
            return *this;
        }

        /** Set radio transmit power in dBm (accepted values are -40, -20, -16, -12, -8, -4, 0, and 4 dBm). */
        Nrf51822BluetoothStack& setTxPowerLevel(int8_t powerLevel);

        int8_t getTxPowerLevel() {
            return _tx_power_level;
        }

        /** Set the minimum connection interval in units of 1.25 ms. */
        Nrf51822BluetoothStack& setMinConnectionInterval(uint16_t connectionInterval_1_25_ms) {
            if (_gap_conn_params.min_conn_interval != connectionInterval_1_25_ms) {
                _gap_conn_params.min_conn_interval = connectionInterval_1_25_ms;
                if (_inited) setConnParams();
            }
            return *this;
        }

        /** Set the maximum connection interval in units of 1.25 ms. */
        Nrf51822BluetoothStack& setMaxConnectionInterval(uint16_t connectionInterval_1_25_ms) {
            if (_gap_conn_params.max_conn_interval != connectionInterval_1_25_ms) {
                _gap_conn_params.max_conn_interval = connectionInterval_1_25_ms;
                if (_inited) setConnParams();
            }
            return *this;
        }

        /** Set the slave latency count. */
        Nrf51822BluetoothStack& setSlaveLatency(uint16_t slaveLatency) {
            if ( _gap_conn_params.slave_latency != slaveLatency ) {
                _gap_conn_params.slave_latency = slaveLatency;
                if (_inited) setConnParams();
            }
            return *this;
        }

        /** Set the connection supervision timeout in units of 10 ms. */
        Nrf51822BluetoothStack& setConnectionSupervisionTimeout(uint16_t conSupTimeout_10_ms) {
            if (_gap_conn_params.conn_sup_timeout != conSupTimeout_10_ms) {
                _gap_conn_params.conn_sup_timeout = conSupTimeout_10_ms;
                if (_inited) setConnParams();
            }
            return *this;
        }

        Nrf51822BluetoothStack& onConnect(const callback_connected_t& callback) {
            //if (callback && _callback_connected) BLE_THROW("Connected callback already registered.");

            _callback_connected = callback;
            if (_inited) setConnParams();
            return *this;
        }
        Nrf51822BluetoothStack& onDisconnect(const callback_disconnected_t& callback) {
            //if (callback && _callback_disconnected) BLE_THROW("Disconnected callback already registered.");

            _callback_disconnected = callback;

            return *this;
        }

        Service& createService() {
            GenericService* svc = new GenericService();
            addService(svc);
            return *svc;
        }

        Service& getService(string name);
        Nrf51822BluetoothStack& addService(Service* svc);

        Nrf51822BluetoothStack& startAdvertising();

        Nrf51822BluetoothStack& stopAdvertising();

        Nrf51822BluetoothStack& onRadioNotificationInterrupt(uint32_t distanceUs, callback_radio_t callback);

        virtual bool connected() {
            return _conn_handle != BLE_CONN_HANDLE_INVALID;
        }
        virtual uint16_t getConnectionHandle() {  // TODO are multiple connections supported?
            return _conn_handle;
        }
        /** Call this as often as possible.  Callbacks on services and characteristics will be invoked from here. */
        void loop();

    protected:
        void setTxPowerLevel();
        void setConnParams();

        void on_ble_evt(ble_evt_t * p_ble_evt);
        void on_connected(ble_evt_t * p_ble_evt);
        void on_disconnected(ble_evt_t * p_ble_evt);

    };

}


#endif /* __BluetoothLE_h */
