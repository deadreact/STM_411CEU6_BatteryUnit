#include <gui/containers/BatteryProgressBar.hpp>
#include <images/BitmapDatabase.hpp>

BatteryProgressBar::BatteryProgressBar()
{

}

void BatteryProgressBar::initialize()
{
    BatteryProgressBarBase::initialize();
}



void BatteryProgressBar::setProgress(uint8_t progress)
{
	if (m_progress != progress)
	{
		m_progress = progress;

		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", (int)progress);
		progress_bg.setBitmap(touchgfx::Bitmap(progress > 20 ? BITMAP_BAR_MEDIUM_FILL_WHITE_ID : BITMAP_BAR_MEDIUM_FILL_RED_ID));
		progress_bg.setWidth(progress);

		invalidateContent();
	}
}
