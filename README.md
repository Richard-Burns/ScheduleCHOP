# ScheduleCHOP

A TouchDesigner CHOP for scheduling events based on the systems time.

The days page lets you set days of the week you want to enable the CHOP.

The Time page lets you set the enable hour, minutes and seconds and disable hour minutes and seconds. You must set the disable value to be higher than the enable hour or the CHOP will not work. If you want to schedule something over midnight you can just use a math after to flip enable and disable.

When enabled the CHOP channel will have a value of 1. When disabled it will be 0.
