close all

data = readmatrix('imu_data.csv');

t = data(:,1);
roll = data(:,2);
pitch = data(:,3);
roll_rough = data(:, 4);
pitch_rough = data(:, 5);




var(roll_rough - mean(roll_rough))
var(pitch_rough - mean(pitch_rough))

figure();
plot(t, pitch); hold on;
plot(t, pitch_rough);
legend('Pitch (Kalman)','Pitch (rough)');
xlabel('Time (s)');
ylabel('Angle (deg)');
grid on;

figure();
plot(t, roll); hold on;
plot(t, -1*roll_rough);
legend('Roll (Kalman)','Roll (rough)');
xlabel('Time (s)');
ylabel('Angle (deg)');
grid on;

