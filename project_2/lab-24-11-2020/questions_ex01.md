# What are the ARP packets and what are they used for?

![image-20201203113802576](C:\Users\gonca\AppData\Roaming\Typora\typora-user-images\image-20201203113802576.png)

Address Resolution Protocol (ARP) is a procedure for mapping an IP address to a permanent physical machine address in a local area network (LAN). The physical machine address is also known as a Media Access Control or MAC address.

When a new computer joins a LAN, it is assigned an unique IP address to user for identification and communication. When an incoming packet destined for a host machine arrives at the gateway, the gateway asks the ARP program to find a MAC address that matches the IP address. The ARP table keeps a record of each IP address and its corresponding MAC address. 

Put in simple:

We have one computer [tux33] with IP address 172.16.30.1 and we want to ping to another computer [tux34] with IP address 172.16.30.254. As we are pinging from tux33 we know its MAC address but we don't know tux34's MAC address, and without it we cannot send any packet. This is where ARP comes in, giving the MAC address corresponding to the IP address requested.

----



# What are the MAC and IP addresses of ARP packets and why?

For the **request ARP packet**, as we stated before, we have **sender's** IP and MAC addresses, and for target we only have IP address, so the ARP packet is a request packet, with target MAC address filled with **zeros**.

![image-20201203115511135](C:\Users\gonca\AppData\Roaming\Typora\typora-user-images\image-20201203115511135.png)

For the **reply ARP packet**, we finally have the MAC address for the machine we asked on the request packet. But on this packet, the target the sender on the request packet, as it should be, as we this packet is targeting the machine who requested the MAC address for this machine.

![image-20201203115905298](C:\Users\gonca\AppData\Roaming\Typora\typora-user-images\image-20201203115905298.png)

---



# What packets does the ping command generate?

Ping command generates an Internet Control Message Protocol (ICMP) packet.

---



# What are the MAC and IP addresses of the ping packets?

Ping packets contain the source and the destination MAC and IP addresses.

---

# How to determine if a receiving Ethernet frame is ARP, IP, ICMP?

We can check the frame type on the **type bytes** of the frame, on our logs we have `0x0800` for the ICMP packets and `0x0806` for the ARP Packets.

# How to determine the length of a receiving frame?

TODO

---

# What is the loopback interface and why is it important?

TODO 

---

