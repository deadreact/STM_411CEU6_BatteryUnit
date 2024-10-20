#ifndef CHARGETIME_HPP
#define CHARGETIME_HPP

#include <gui_generated/containers/ChargeTimeBase.hpp>

class ChargeTime : public ChargeTimeBase
{
	enum class Format
	{
		Full,
		Hours,
		Minutes,
		Minimum
	};
public:
    ChargeTime();
    virtual ~ChargeTime() {}

    virtual void initialize();
    void setValue(int val);
    void setFormat(Format format);
    void setTime(int h, int m);
protected:
    void applyFormat();
protected:
    int m_value{0};
    Format m_format{Format::Hours};
    constexpr static const int kYellowTreshold = 1800;
    constexpr static const int kRedTreshold = 360;
};

#endif // CHARGETIME_HPP
