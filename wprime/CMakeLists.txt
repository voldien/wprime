	# Add execute and link library.
	ADD_EXECUTABLE(wprime ${CMAKE_CURRENT_SOURCE_DIR}/wprime.c )
	TARGET_LINK_LIBRARIES(wprime -lm)

	# Install program.
	INSTALL (TARGETS wprime DESTINATION bin)