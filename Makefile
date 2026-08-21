CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2
IFLAGS   := -Iinclude
OUTDIR   := build
TARGET   := $(OUTDIR)/LibrarySystem

SOURCES  := \
    src/utils/DateTime.cpp \
    src/utils/Crypto.cpp \
    src/utils/StringUtils.cpp \
    src/utils/Terminal.cpp \
    src/utils/TableFormatter.cpp \
    src/models/Book.cpp \
    src/models/User.cpp \
    src/models/Loan.cpp \
    src/models/Reservation.cpp \
    src/storage/CsvEngine.cpp \
    src/storage/BookRepository.cpp \
    src/storage/UserRepository.cpp \
    src/storage/LoanRepository.cpp \
    src/storage/ReservationRepository.cpp \
    src/storage/AuditLogger.cpp \
    src/services/BookService.cpp \
    src/services/AuthService.cpp \
    src/services/MemberService.cpp \
    src/services/CirculationService.cpp \
    src/services/ReportService.cpp \
    src/ui/LibraryApp.cpp \
    main.cpp

.PHONY: all clean run

all: $(OUTDIR) $(TARGET)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(IFLAGS) $^ -o $@
	@echo "Build successful: $(TARGET)"

run: all
	./$(TARGET)

clean:
	rm -rf $(OUTDIR)
