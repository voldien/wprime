PROJECT(resume_CV NONE)
CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
# Set path for CMake
SET(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    ${CMAKE_MODULE_PATH}
)

SET(LATEX_OUTPUT_PATH Proof)
SET(OUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
SET(BIB_REFERENCE_PATH ${CMAKE_SOURCE_DIR}/references.bib)
FILE(MAKE_DIRECTORY ${OUT_DIRECTORY})

FIND_PACKAGE(PythonInterp)
FIND_PACKAGE(LATEX)
IF(LATEX_FOUND)
	IF(LUALATEX__FOUND)
		MESSAGE(STATUS "lualatex found - ${LUALATEX_COMPILER}")
	ENDIF()
	IF(BIBTEX_FOUND)
		MESSAGE(STATUS "bibtex found - ${BIBTEX_COMPILER}")
	ENDIF()
	IF(MAKEINDEX_FOUND)
		MESSAGE(STATUS "makeindex found - ${MAKEINDEX_COMPILER}")
	ENDIF()
ELSE()
	MESSAGE(ERROR "No latex tools found!")
ENDIF()

IF(PYTHONINTERP_FOUND)
	FILE(GLOB_RECURSE bib_references RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bib/*.bib)
	SET(APPEND_UTIL_SCRIPT ${CMAKE_SOURCE_DIR}/utils/append_with_delimiter.py)
	ADD_CUSTOM_TARGET( cv-append-bib
            COMMENT "Creating appended bib file."
            COMMAND ${PYTHON_EXECUTABLE} ${APPEND_UTIL_SCRIPT} ${bib_references} ${BIB_REFERENCE_PATH}
            DEPENDS ${APPEND_UTIL_SCRIPT})
ELSE()
	MESSAGE(WARNING "May not be able to generate the main bib file!")
ENDIF()


SET( MAIN_TEX_BASE_FILENAME "proof")
SET( MAIN_TEX "${CMAKE_CURRENT_SOURCE_DIR}/${MAIN_TEX_BASE_FILENAME}.tex")
SET( MAIN_IDX "${LATEX_OUTPUT_PATH}/${MAIN_TEX_BASE_FILENAME}.idx")
SET( MAIN_AUX "${LATEX_OUTPUT_PATH}/${MAIN_TEX_BASE_FILENAME}.aux")
SET( WORKINGDIR "${CMAKE_CURRENT_SOURCE_DIR}")

# First pass.
ADD_CUSTOM_TARGET( cv-latex-prebuild
		COMMAND ${LUALATEX_COMPILER}  -output-directory ${OUT_DIRECTORY} -draftmode -interaction=nonstopmode ${MAIN_TEX}
		COMMENT "Starting Prebuild."
		WORKING_DIRECTORY ${WORKINGDIR}
		DEPENDS ${MAIN_TEX})

# Generate the indices.
ADD_CUSTOM_TARGET( cv-latex-makeindex
			COMMAND ${MAKEINDEX_COMPILER} ${MAIN_IDX}
			COMMENT "Read and create indices with ${MAIN_IDX}."
			DEPENDS ${MAIN_IDX})
ADD_DEPENDENCIES(cv-latex-makeindex cv-latex-prebuild)

# Generate what citation found in the latex file.
ADD_CUSTOM_TARGET( cv-latex-bibreferences
			COMMAND ${BIBTEX_COMPILER} ${CMAKE_CURRENT_BINARY_DIR}/proof.aux
			COMMENT "Read and create main bib references file."
			DEPENDS  ${CMAKE_CURRENT_BINARY_DIR}/proof.aux)
ADD_DEPENDENCIES( cv-latex-bibreferences cv-latex-prebuild)	

# Second pass - generate the final pdf.
ADD_CUSTOM_TARGET( cv-latex-pdf	
			COMMAND ${LUALATEX_COMPILER} -output-directory ${OUT_DIRECTORY} ${MAIN_TEX} 
			COMMAND cp ${OUT_DIRECTORY}/proof.pdf ${CMAKE_SOURCE_DIR}/proof.pdf # Copy the file to the root directory.
			WORKING_DIRECTORY ${WORKINGDIR}
			COMMENT "Create pdf"
			DEPENDS  ${MAIN_TEX})
ADD_DEPENDENCIES( cv-latex-pdf cv-latex-prebuild cv-latex-bibreferences)

ADD_CUSTOM_TARGET(cv-all-formats ALL)
ADD_DEPENDENCIES( cv-all-formats cv-latex-pdf)
