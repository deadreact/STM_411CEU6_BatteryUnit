#include <gui/containers/BatteryMainProgressBar.hpp>
#include <images/BitmapDatabase.hpp>

BatteryMainProgressBar::BatteryMainProgressBar()
{

}

void BatteryMainProgressBar::initialize()
{
    BatteryMainProgressBarBase::initialize();
}


void BatteryMainProgressBar::setProgress(uint8_t progress)
{
	if (m_progress != progress)
	{
		m_progress = progress;

		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", (int)progress);
		progress_bg.setBitmap(touchgfx::Bitmap(progress > 20 ? BITMAP_BAR_BIG_FILL_WHITE_ID : BITMAP_BAR_BIG_FILL_RED_ID));
		progress_bg.setWidth((uint16_t)progress * 2.9);

		invalidateContent();
	}
}
