#!/bin/bash

# Set the Qt platform plugin to xcb to avoid "wayland" plugin issues
export QT_QPA_PLATFORM=xcb

./ns3 run "scratch/1905117_1.cc --bttlnkRate=5 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=15 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=35 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=55 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=125 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=200 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=250 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=275 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=300 --plossRate=6 --exp=1 --tcp2="ns3::TcpHighSpeed""

./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=2 --exp=2 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=3 --exp=2 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=4 --exp=2 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=5 --exp=2 --tcp2="ns3::TcpHighSpeed""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=6 --exp=2 --tcp2="ns3::TcpHighSpeed""

./ns3 run "scratch/1905117_1.cc --bttlnkRate=5 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=15 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=35 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=55 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=125 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=200 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=250 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=275 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=300 --plossRate=6 --exp=1 --tcp2="ns3::TcpAdaptiveReno""

./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=2 --exp=2 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=3 --exp=2 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=4 --exp=2 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=5 --exp=2 --tcp2="ns3::TcpAdaptiveReno""
./ns3 run "scratch/1905117_1.cc --bttlnkRate=50 --plossRate=6 --exp=2 --tcp2="ns3::TcpAdaptiveReno""

gnuplot -e "set terminal pngcairo enhanced font 'arial,10' fontscale 1.0 size 800,600;
            set output 'scratch/Congestion Window vs Time 2.png';
            set title 'Congestion Window vs Time (Tcp2: TcpAdaptiveReno)';
            set ylabel 'Congestion Window';
            set xlabel 'Time';
            plot 'task1/flow1ns3::TcpAdaptiveReno.cwnd' using 1:2 with lines title 'tcpnewreno';
            replot 'task1/flow2ns3::TcpAdaptiveReno.cwnd' using 1:2 with lines title 'tcpadaptive';"


gnuplot -e "set terminal pngcairo enhanced font 'arial,10' fontscale 1.0 size 800,600;
            set output 'scratch/Congestion Window vs Time 1.png';
            set title 'Congestion Window vs Time (Tcp2: TcpHighSpeed)';
            set ylabel 'Congestion Window';
            set xlabel 'Time';
            plot 'task1/flow1ns3::TcpHighSpeed.cwnd' using 1:2 with lines title 'tcpnewreno';
            replot 'task1/flow2ns3::TcpHighSpeed.cwnd' using 1:2 with lines title 'tcphighspeed';"


gnuplot -e "set terminal pngcairo enhanced font 'arial,10' fontscale 1.0 size 800,600;
            set output 'scratch/Throughput vs BottleNeck Data Rate 2.png';
            set title 'Throughput vs BottleNeck Data Rate (Tcp2: TcpAdaptiveReno)';
            set ylabel 'Throughput';
            set xlabel 'BottleNeck Data Rate';
            plot 'task1/data_1ns3::TcpAdaptiveReno.txt' using 1:3 with lines title 'tcpnewreno';
            replot 'task1/data_1ns3::TcpAdaptiveReno.txt' using 1:4 with lines title 'tcpadaptive'"

gnuplot -e "set terminal pngcairo enhanced font 'arial,10' fontscale 1.0 size 800,600;
            set output 'scratch/Throughput vs BottleNeck Data Rate 1.png';
            set title 'Throughput vs BottleNeck Data Rate (Tcp2: TcpHighSpeed)';
            set ylabel 'Throughput';
            set xlabel 'BottleNeck Data Rate';
            plot 'task1/data_1ns3::TcpHighSpeed.txt' using 1:3 with lines title 'tcpnewreno';
            replot 'task1/data_1ns3::TcpHighSpeed.txt' using 1:4 with lines title 'tcphighspeed'" 

gnuplot -e "set terminal pngcairo enhanced font 'arial,10' fontscale 1.0 size 800,600;
            set output 'scratch/Throughput vs Packet Loss Rate 2.png';
            set title 'Throughput vs Packet Loss Rate (Tcp2: TcpAdaptiveReno)';
            set ylabel 'Throughput';
            set xlabel 'Packet Loss Rate';
            plot 'task1/data_2ns3::TcpAdaptiveReno.txt' using 2:3 with lines title 'tcpnewreno';
            replot 'task1/data_2ns3::TcpAdaptiveReno.txt' using 2:4 with lines title 'tcpadaptive'"          

gnuplot -e "set terminal pngcairo enhanced font 'arial,10' fontscale 1.0 size 800,600;
            set output 'scratch/Throughput vs Packet Loss Rate 1.png';
            set title 'Throughput vs Packet Loss Rate (Tcp2: TcpHighSpeed)';
            set ylabel 'Throughput';
            set xlabel 'Packet Loss Rate';
            plot 'task1/data_2ns3::TcpHighSpeed.txt' using 2:3 with lines title 'tcpnewreno';
            replot 'task1/data_2ns3::TcpHighSpeed.txt' using 2:4 with lines title 'tcphighspeed'" 



#gnuplot "1905117.gp"
