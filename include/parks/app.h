#pragma once

#include <iostream>
#include <parks/traits.h>
#include <vector>
#include "blt/std/logging.h"
#include "status.h"

namespace parks {
    namespace Properties {
        enum PROPERTIES {
            // The display window's title
            WINDOW_TITLE,
            // the initial display width/height. This value does not change if the window is resized. Use Window::getWidth and Window::getHeight
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            WINDOW_RESIZABLE,
            RENDER_MODE,
        };

        class Value_t {
        public:
            Value_t() = default;
            Value_t(const Value_t& value) = delete;
            virtual ~Value_t() = default;
            virtual void writeValue(std::ostream& ostream) = 0;
            virtual void readValue(std::istream istream) = 0;

            template<typename T>
            inline T* as(){
                return dynamic_cast<T*>(this);
            }
        };

        template<typename T, typename std::enable_if<parks::is_streamable<T>::value>::type* = nullptr>
        class Value : public Value_t {
        private:
            T* m_Value;
        public:
            explicit Value(const T& v): m_Value(new T(v)) {}
            explicit Value(T* v): m_Value(v) {}
            Value() = default;
            inline const T& getValue(){ return *m_Value; }
            inline void writeValue(std::ostream& ostream) final {
                ostream << *m_Value;
            }
            inline void readValue(std::istream istream) final {
                istream >> *m_Value;
            }
            ~Value() override {
                delete m_Value;
            }
        };
    }
    class Settings {
    public:
        Settings() = default;
        ~Settings() {
            for (auto* p : values)
                delete p;
        }
        [[nodiscard]] Properties::Value_t* getProperty(Properties::PROPERTIES property) const {
            //BLT_TRACE("Getting Property information (%d)", property);
            //BLT_TRACE("Property: %d, Value: %d (Getting)", property, values[property]);
            if (values.size() <= property)
                return nullptr;
            return values[property];
        }
        void setProperty(Properties::PROPERTIES property, Properties::Value_t* value) {
            BLT_TRACE("Setting property information (%d)", property);
            BLT_TRACE("Values size / capacity (before): %d/%d", values.size(), values.capacity());
            BLT_TRACE("Property: %d, Value: %d (Setting)", property, value);
            if (values.capacity() <= property)
                values.resize(property * 2);
            values[property] = value;
            BLT_TRACE("Values size / capacity (after): %d/%d", values.size(), values.capacity());
            BLT_TRACE("Dumping values:");
            BLT_TRACE("---------------------");
            for (unsigned int i = 0; i < values.size(); i++)
                BLT_TRACE("Value (%d): %d", i, values[i]);
            BLT_TRACE("---------------------");
        }
    private:
        std::vector<Properties::Value_t*> values;
    };
}