#ifndef CHARGETIME_HPP
#define CHARGETIME_HPP

#include <gui_generated/containers/ChargeTimeBase.hpp>

class ChargeTime : public ChargeTimeBase
{
	enum class Format
	{
		Hidden,
		Full,
		Hours,
		Minutes,
		Minimum
//		Maximum
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
    Format m_format{Format::Hidden};
    constexpr static const int kYellowTreshold = 30;
    constexpr static const int kRedTreshold = 10;
};

#endif // CHARGETIME_HPP
