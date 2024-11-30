#!/bin/bash
./ns3 run "scratch/1905117_1.cc --nWifi=20 --num_flow=10 --nPackets=20 --speed=10 --coverageArea=2"
./ns3 run "scratch/1905117_1.cc --nWifi=40 --num_flow=20 --nPackets=20 --speed=10 --coverageArea=2"
./ns3 run "scratch/1905117_1.cc --nWifi=60 --num_flow=30 --nPackets=20 --speed=10 --coverageArea=2"
./ns3 run "scratch/1905117_1.cc --nWifi=80 --num_flow=40 --nPackets=20 --speed=10 --coverageArea=2"
./ns3 run "scratch/1905117_1.cc --nWifi=100 --num_flow=50 --nPackets=20 --speed=10 --coverageArea=2"

gnuplot -e "set output "scratch/output.png";\
            set title "nodevsthroughputgraph";\
            set xlabel "node";\
            set ylabel "throughput";\
            plot "scratch/output.dat" using 1:2 with lines title "wiegweuifwweuif""
#!/bin/bash
./ns3 run "scratch/1905117_2.cc --nWifi=20 --num_flow=10 --nPackets=20 --speed=10 --coverageArea=2"
./ns3 run "scratch/1905117_2.cc --nWifi=40 --num_flow=20 --nPackets=20 --speed=10 --coverageArea=2"
./ns3 run "scratch/1905117_2.cc --nWifi=60 --num_flow=30 --nPackets=20 --speed=10 --coverageArea=2"
./ns3 run "scratch/1905117_2.cc --nWifi=80 --num_flow=40 --nPackets=20 --speed=10 --coverageArea=2"
./ns3 run "scratch/1905117_2.cc --nWifi=100 --num_flow=50 --nPackets=20 --speed=10 --coverageArea=2"

gnuplot -e "set output "scratch/output2.png";\
            set title "nodevsthroughputgraph";\
            set xlabel "node";\
            set ylabel "throughput";\
            plot "scratch/output2.dat" using 1:2 with lines title "wiegweuifwweuif""
