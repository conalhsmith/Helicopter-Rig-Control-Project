The goal of this project was to program a remote controlled helicopter so that a “pilot” can fly a sortie, comprised of the following component tasks:

Find a direction in which to take off.

Rise to an altitude where stable, manoeuvrable flight can be maintained.

Rotate around a fixed position and over several incremental steps.

Land back at base, “parking” in a home position

The main project output was to program a TIVA board that contains a small, interrupt-driven, real-time kernel able to control the lift-off, hover, heading, and landing of a helicopter mounted on a remote rig. The project also involves a PID control system and PWM module to control the helicopter altitude and yaw, which is monitored via interrupts and quadrature encoders.

I would like to disclose that this is not finished and I am currently working on further improvements such as an interrupt driven kernal rather than a round robin.
