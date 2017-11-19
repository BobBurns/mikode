#include "common.h"
#include "instructions.h"
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int write_listing(line **in, uint8_t ***dp, int count, char *name)
{
	FILE *listing;
	char lname[32];
	strncpy(lname, name, 32);

	char *list_name = strcat(lname, ".list");
	if (list_name == NULL)
		return -1;

	if ((listing = fopen(list_name, "w+")) == NULL) {
		perror("fopen ");
		return -1;
	}
	
	int j , k = 0;

	fprintf(listing, "Listing for %s.\n\n", name);
	for (j = 0; j < count; j++)
	{
		fprintf(listing, "0x%04x\t", (*in)[j].address);
		/*fprintf(listing, "%s\t", (*in)[j].label);*/
		if ((*in)[j].dataidx > 0) {
			for (k = 0; k < (*in)[j].size; k++)
				fprintf(listing, "%02x ", (*dp)[(*in)[j].dataidx-1][k]);
		}
		if ((*in)[j].opcode != 0xffff)
			fprintf(listing, "%04x  ", (*in)[j].opcode);
		else 
			fprintf(listing, "      ");
		if ((*in)[j].type == DIR_32 || (*in)[j].type == LD_DIR 
				|| (*in)[j].type == STR_DIR)
			fprintf(listing, "%04x  ", (*in)[j].opextra);
		else 
			fprintf(listing, "      ");
		fprintf(listing, "\t%s", (*in)[j].text);
	}
	fclose(listing);
	return 0;
}

int write_asm(line **in, uint8_t ***dp, int count, char *name)
{
	
	int fd, len, n, ret;
	char lname[32];
	strncpy(lname, name, 32);

	len = strlen(lname);
	if (strncmp(&lname[len-4], ".asm", 4)) {
		fprintf(stderr, "must be .asm file\n");
		return -1;
	}
	lname[len-4] = '\0';
	printf("writing %s...\n", lname);

	if ((fd = open(lname, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
		perror("open ");
		return -1;
	}
	
	int j , k = 0;

	char header[HOFF] = {'b', 'a', 's', 'm', 0x00, 0x00, 0x00, 0x00 };
	n = write(fd, &header, sizeof(header));
	if (n != sizeof(header)) {
		fprintf(stderr, "problem writing header");
		goto write_fail;
	}
	
	uint8_t byte_buf;
	uint16_t short_buf;
	for (j = 0; j < count; j++)
	{
		ret = lseek(fd, (*in)[j].address + HOFF, SEEK_SET);
		if (ret < 0) {
			perror("lseek:");
			return -1;
		}
		if ((*in)[j].dataidx > 0) {
			for (k = 0; k < (*in)[j].size; k++) {
				byte_buf = (*dp)[(*in)[j].dataidx-1][k];
				n = write(fd, &byte_buf, 1);
				if (n < 0) {
					fprintf(stderr, "problem writing bytes\n");
					goto write_fail;
				}
			}
		}
		if ((*in)[j].opcode != NOCODE) {
			short_buf = (*in)[j].opcode;
			n = write(fd, &short_buf, 2);
			if (n < 0) {
				fprintf(stderr, "problem writing opcode\n");
				goto write_fail;
			}
		}
		if ((*in)[j].type == DIR_32 || (*in)[j].type == LD_DIR 
				|| (*in)[j].type == STR_DIR) {
			short_buf = (*in)[j].opextra;
			n = write(fd, &short_buf, 2);
			if (n < 0) {
				fprintf(stderr, "problem writing operand 2\n");
				goto write_fail;
			}
		}
		
	}
	sync();
write_fail:
	close(fd);
	return 0;
}
