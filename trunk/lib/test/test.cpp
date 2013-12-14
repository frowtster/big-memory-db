#include "gtest/gtest.h"
#include "OmniPacket.h"

TEST(OmniPacket, Parse)
{
	OmniPacket packet;

	// no Cmd
	strcpy( packet.mBuffer, "MRI\r\n");
	EXPECT_EQ( packet.Parse(), FALSE );

	// no Trid
	strcpy( packet.mBuffer, "MRIN \r\n");
	EXPECT_EQ( packet.Parse(), FALSE );

	// no Header
	strcpy( packet.mBuffer, "MRIN 0\r\n");
	EXPECT_EQ( packet.Parse(), FALSE );
	strcpy( packet.mBuffer, "MRIN 0 \r\n");
	EXPECT_EQ( packet.Parse(), FALSE );

	// no Length
	strcpy( packet.mBuffer, "MRIN 0 frowt\r\n");
	EXPECT_EQ( packet.Parse(), FALSE );
	strcpy( packet.mBuffer, "MRIN 0 frowt \r\n");
	EXPECT_EQ( packet.Parse(), FALSE );
	strcpy( packet.mBuffer, "MRIN 0 0\r\n");
	EXPECT_EQ( packet.Parse(), FALSE );

	// success
	strcpy( packet.mBuffer, "MRIN 0 frowt 0\r\n");
	EXPECT_EQ( packet.Parse(), TRUE );
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

